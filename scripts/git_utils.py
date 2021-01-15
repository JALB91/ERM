import subprocess

def log_wait(message):
    print(message)

def is_pre_commit_stash(stash):
    return stash.find('PRE_COMMIT_STASH') != -1

def get_stashes_list():
    result = subprocess.run(['git', 'stash', 'list'], stdout=subprocess.PIPE)
    stashes = result.stdout.decode('utf-8').split('\n')
    return [i for i in stashes if i]

def cleanup_pre_commit_stashes():
    done = False
    stashes = get_stashes_list()
    log_wait("Cleaning up stashes: " + str(stashes))
    while not done and stashes:
        for stash in stashes:
            if is_pre_commit_stash(stash):
                log_wait("Cleaning up stash: " + str(stash))
                subprocess.run(['git', 'stash', 'drop', stash[0:stash.index(':')]], stdout=subprocess.PIPE)
                stashes = get_stashes_list()
                done = False
                break
            else:
                log_wait("Not cleaning up stash: " + str(stash))
                done = True
    log_wait("Stashes cleaned up: " + str(get_stashes_list()))

def has_valid_pre_commit_stash():
    stashes = get_stashes_list()
    return stashes and is_pre_commit_stash(stashes[0])

def pop_pre_commit_stash_if_valid():
    if has_valid_pre_commit_stash():
        log_wait("Popping pre commit stash: " + str(get_stashes_list()))
        subprocess.run(['git', 'stash', 'pop'], stdout=subprocess.PIPE)
        return True
    return False

def get_files_to_commit():
    result = subprocess.run(['git', 'diff', '--cached', '--name-only'], stdout=subprocess.PIPE)
    toCommit = result.stdout.decode('utf-8').split('\n')
    return [i for i in toCommit if i]

def get_files_with_local_changes():
    result = subprocess.run(['git', 'diff', '--name-only'], stdout=subprocess.PIPE)
    localChanges = result.stdout.decode('utf-8').split('\n')
    return [i for i in localChanges if i]

def stash_with_options(options):
    log_wait("Stashing with options: " + str(options))
    argsList = ['git', 'stash', 'push']
    argsList.extend(options)
    subprocess.run(argsList, stdout=subprocess.PIPE)

def add_to_index(filePath):
    log_wait("Adding to index: " + str(filePath))
    subprocess.run(['git', 'add', filePath], stdout=subprocess.PIPE)