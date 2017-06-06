from __future__ import print_function
import rif.dtypes

from rif_cpp.test import pandas_interop as pi

import numpy as np
import xarray as xr


def test_can_treat_xr_as_dict():
    with rif.dtypes.rif_ops_disable():  # this may indicate sketchiness
        data = xr.DataArray(np.random.randn(2, 3), coords={'x': ['a', 'b']}, dims=('x', 'y'))
        ds = xr.Dataset({'foo': data, 'bar': ('x', [1, 2]), 'baz': np.pi})
        sp = pi.print_dict(ds)  # this dies with rif.dtypes?
        print(sp)
        assert 17 == len(sp.splitlines())
        kn, totlen = pi.key_names_and_len(ds)
        assert totlen == 11
