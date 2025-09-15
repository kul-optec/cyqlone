import cyqlone


def test_version():
    assert cyqlone.__version__ == cyqlone.__c_version__
    cyqlone.build_time
