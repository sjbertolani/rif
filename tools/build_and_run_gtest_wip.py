#!/usr/bin/env python

"""build and run gtest_wip c++ exe"""


from builtins import *

import os
import sys

os.sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from build_utils import get_proj_root, get_build_dir, rebuild_fast, add_to_pypath
from build_utils import remove_installed_rif


if __name__ == '__main__':
    # remove_installed_rif()
    if rebuild_fast(target='gtest_wip', cfg='Release'):
        sys.exit(-1)
    os.system(get_build_dir('temp', cfg='Release') + '/gtest_wip')
