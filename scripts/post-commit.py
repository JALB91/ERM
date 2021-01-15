import sys, subprocess
from git_utils import pop_pre_commit_stash_if_valid

def post_commit():
    pop_pre_commit_stash_if_valid()

if (__name__ == '__main__'):
    post_commit()
    sys.exit(0)
else:
    sys.exit(1)
