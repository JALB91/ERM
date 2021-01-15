import sys, subprocess
from git_utils import cleanup_pre_commit_stashes, stash_with_options, add_to_index, pop_pre_commit_stash_if_valid, get_files_to_commit, get_files_with_local_changes

def format_file(filePath):
    subprocess.run(['clang-format', '-i', '-style=file', filePath], stdout=subprocess.PIPE)

def pre_commit():
    cleanup_pre_commit_stashes()
    toCommit = get_files_to_commit()
    localChanges = get_files_with_local_changes()

    needStash = False

    for filePath in toCommit:
        if filePath in localChanges and (filePath.endswith(".h") or filePath.endswith(".cpp")):
            needStash = True
            stash_with_options(['--keep-index', '--include-untracked', '-m PRE_COMMIT_STASH'])
            break

    for filePath in toCommit:
        if filePath and (filePath.endswith('.h') or filePath.endswith('.cpp')):
            format_file(filePath)
            add_to_index(filePath)

    toCommit = get_files_to_commit()

    if not toCommit:
        if needStash:
            pop_pre_commit_stash_if_valid()
        print("Nothing to commit")
        sys.exit(1)
        
if (__name__ == '__main__'):
    pre_commit()
    sys.exit(0)
else:
    sys.exit(1)
