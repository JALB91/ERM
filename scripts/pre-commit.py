import sys, subprocess

def pre_commit():
    result = subprocess.run(['git', 'diff', '--cached', '--name-only'], stdout=subprocess.PIPE)
    index = result.stdout.decode('utf-8').split('\n')

    result = subprocess.run(['git', 'diff', '--name-only'], stdout=subprocess.PIPE)
    local = result.stdout.decode('utf-8').split('\n')

    need_stash = False

    for diff in index:
        if diff in local:
            need_stash = True
            subprocess.run(['git', 'stash', 'push', '--keep-index', '--include-untracked', '-m PRE_COMMIT_STASH'], stdout=subprocess.PIPE)
            break

    for diff in index:
        if diff.count('include/') > 0 or diff.count('src/') > 0:
            subprocess.run(['clang-format', '-i', '-style=file', diff], stdout=subprocess.PIPE)
            subprocess.run(['git', 'add', diff], stdout=subprocess.PIPE)

    result = subprocess.run(['git', 'diff', '--cached', '--name-only'], stdout=subprocess.PIPE)
    index = result.stdout.decode('utf-8')

    if not index:
        if need_stash:
            subprocess.run(['git', 'stash', 'pop'], stdout=subprocess.PIPE)
        print("Nothing to commit")
        sys.exit(1)
        

if (__name__ == '__main__'):
    pre_commit()
    sys.exit(0)
else:
    sys.exit(1)
