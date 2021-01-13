import sys, subprocess

def post_commit():
    result = subprocess.run(['git', 'stash', 'list'], stdout=subprocess.PIPE)
    stashes = result.stdout.decode('utf-8').split('\n')

    if stashes and stashes[0].count('PRE_COMMIT_STASH') > 0:
        subprocess.run(['git', 'stash', 'pop'], stdout=subprocess.PIPE)

if (__name__ == '__main__'):
    post_commit()
    sys.exit(0)
else:
    sys.exit(1)
