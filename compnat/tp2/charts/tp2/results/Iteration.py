# automatically generated by the FlatBuffers compiler, do not modify

# namespace: results

import flatbuffers

class Iteration(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsIteration(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = Iteration()
        x.Init(buf, n + offset)
        return x

    # Iteration
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

# /// Global bests, one float for each execution.
    # Iteration
    def GlobalBests(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            a = self._tab.Vector(o)
            return self._tab.Get(flatbuffers.number_types.Float32Flags, a + flatbuffers.number_types.UOffsetTFlags.py_type(j * 4))
        return 0

    # Iteration
    def GlobalBestsLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

# /// Local bests, one float for each execution.
    # Iteration
    def LocalBests(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            a = self._tab.Vector(o)
            return self._tab.Get(flatbuffers.number_types.Float32Flags, a + flatbuffers.number_types.UOffsetTFlags.py_type(j * 4))
        return 0

    # Iteration
    def LocalBestsLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

# /// Local worsts, one float for each execution.
    # Iteration
    def LocalWorsts(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(8))
        if o != 0:
            a = self._tab.Vector(o)
            return self._tab.Get(flatbuffers.number_types.Float32Flags, a + flatbuffers.number_types.UOffsetTFlags.py_type(j * 4))
        return 0

    # Iteration
    def LocalWorstsLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(8))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

def IterationStart(builder): builder.StartObject(3)
def IterationAddGlobalBests(builder, globalBests): builder.PrependUOffsetTRelativeSlot(0, flatbuffers.number_types.UOffsetTFlags.py_type(globalBests), 0)
def IterationStartGlobalBestsVector(builder, numElems): return builder.StartVector(4, numElems, 4)
def IterationAddLocalBests(builder, localBests): builder.PrependUOffsetTRelativeSlot(1, flatbuffers.number_types.UOffsetTFlags.py_type(localBests), 0)
def IterationStartLocalBestsVector(builder, numElems): return builder.StartVector(4, numElems, 4)
def IterationAddLocalWorsts(builder, localWorsts): builder.PrependUOffsetTRelativeSlot(2, flatbuffers.number_types.UOffsetTFlags.py_type(localWorsts), 0)
def IterationStartLocalWorstsVector(builder, numElems): return builder.StartVector(4, numElems, 4)
def IterationEnd(builder): return builder.EndObject()