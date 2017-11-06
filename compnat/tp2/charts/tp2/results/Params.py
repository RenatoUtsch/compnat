# automatically generated by the FlatBuffers compiler, do not modify

# namespace: results

import flatbuffers

# /// Parameters used during execution.
class Params(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsParams(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = Params()
        x.Init(buf, n + offset)
        return x

    # Params
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

# /// Global seed used to seed the executions.
    # Params
    def Seed(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Uint32Flags, o + self._tab.Pos)
        return 0

# /// Number of executions.
    # Params
    def NumExecutions(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Uint32Flags, o + self._tab.Pos)
        return 0

# /// Number of iterations.
    # Params
    def NumIterations(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(8))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Uint32Flags, o + self._tab.Pos)
        return 0

# /// Number of ants.
    # Params
    def NumAnts(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(10))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Uint32Flags, o + self._tab.Pos)
        return 0

# /// decay.
    # Params
    def Decay(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(12))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Float32Flags, o + self._tab.Pos)
        return 0.0

def ParamsStart(builder): builder.StartObject(5)
def ParamsAddSeed(builder, seed): builder.PrependUint32Slot(0, seed, 0)
def ParamsAddNumExecutions(builder, numExecutions): builder.PrependUint32Slot(1, numExecutions, 0)
def ParamsAddNumIterations(builder, numIterations): builder.PrependUint32Slot(2, numIterations, 0)
def ParamsAddNumAnts(builder, numAnts): builder.PrependUint32Slot(3, numAnts, 0)
def ParamsAddDecay(builder, decay): builder.PrependFloat32Slot(4, decay, 0.0)
def ParamsEnd(builder): return builder.EndObject()