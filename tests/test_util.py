from fourbraillebars.util import FreeMem_m, CPU_usage, NvidiaGpuUsage

# write tests for the imported NamedTuple classes, write them in the style of pytest tests.

def test_FreeMem_m():
    fmm = FreeMem_m.from_call()
    assert fmm.mem.free > 1
    assert fmm.mem.total > fmm.mem.free

def test_CPU_usage():
    cpu = CPU_usage.from_call()
    for attr in 'user system nice idle wait_io hi si st'.split():
        assert 0 <= getattr(cpu, attr) <= 100.0


def test_NvidiaGpuUsage():
    gpu = NvidiaGpuUsage.from_call()
    assert gpu.mem_use < gpu.mem_tot
