import ScoreDraft
from ScoreDraftNotes import *
from math import pow

# 五度相生的频率比 C #C D #D E F #F G #G A #A B
Freqs_five = [1.0, 3**7/2**11, 3**2/2**3, 3**9/2**14, 3**4/2**6, 3**11/2**17, 3**6/2**9, 3/2, 3**8/2**12, 3**3/2**4, 3**10/2**15, 3**5/2**7]
# 纯律的频率比，采用的是网上提供的频率比数值
Freqs_pure = [1.0, 16/15, 9/8, 6/5, 5/4, 4/3, 45/32, 3/2, 8/5, 5/3, 9/5, 15/8]
# 十二平均率频率比
Freqs_eq = [pow(2, f/12) for f in range(12)]

Freqs_options = {"five":Freqs_five, "pure":Freqs_pure, "eq":Freqs_eq}
Freqs = Freqs_five
# 认为8分音符时长为24个单位，用第五个八度（从1开始数的）作为中央C
# 音符使用一个tuple表示，第一个元素为相对频率比，第二个元素为时值
def note(octave, freq, duration):
    return (freq*(2.0**(octave-5.0)), duration)

# 以下为do #do re #re mi fa #fa so #so la #la ti的8分音符实现

def do(octave=5, duration=24):
    return note(octave,Freqs[0],duration)

def up_do(octave=5, duration=24):
    return note(octave, Freqs[1], duration)

def re(octave=5, duration=24):
    return note(octave,Freqs[2],duration)

def up_re(octave=5, duration=24):
    return note(octave, Freqs[3], duration)

def mi(octave=5, duration=24):
    return note(octave,Freqs[4],duration)

def fa(octave=5, duration=24):
    return note(octave,Freqs[5],duration)

def up_fa(octave=5, duration=24):
    return note(octave, Freqs[6], duration)

def so(octave=5, duration=24):
    return note(octave,Freqs[7],duration)

def up_so(octave=5, duration=24):
    return note(octave, Freqs[8], duration)

def la(octave=5, duration=24):
    return note(octave,Freqs[9],duration)

def up_la(octave=5, duration=24):
    return note(octave, Freqs[10], duration)

def ti(octave=5, duration=24):
    return note(octave,Freqs[11],duration)

# 八分休止符的实现
def BL(duration=24):
    return (-1.0, duration)

# 回退，用于使用和弦
def BK(duration=24):
    return (-1.0, -duration)

# 时值减半，用于实现16分音符
def half(_note):
    return (_note[0], _note[1]/2)

# 时值加倍，用于实现4分音符
def double(_note):
    return (_note[0], _note[1]*2)

# 符点的作用
def dot(_note, delta=1):
    return (_note[0] * 2, _note[1])

# 上方符点
def up_dot(_note):
    return dot(_note)

# 下方符点
def down_dot(_note):
    return dot(_note, -1)

doc = ScoreDraft.Document()

def get_seq1():
    # 下面是各个小节
    part1 = [re(), up_do(), re(), down_dot(double(ti())), up_do(), re(), mi(), up_fa()]
    part2 = part1
    part3 = [ti(), so(), mi(), double(up_do()), re(), so(), mi(), up_do()]
    part4 = [down_dot(double(up_la())), down_dot(ti()), mi(), up_do(), down_dot(up_la()), re(), down_dot(ti()), down_dot(up_so())]
    part5 = [down_dot(half(ti())), down_dot(half(up_la())), down_dot(half(up_so())), down_dot(half(ti())), down_dot(up_la()), half(up_do()),
        down_dot(half(ti())), down_dot(half(up_la())), half(up_do()), down_dot(ti()), half(re()), half(up_do()), down_dot(half(ti())), half(re()),
        half(up_do()), half(mi())]
    part6 = part1
    part7 = part1
    part8 = [ti(), up_so(), fa(), double(up_do()), double(re()), up_so(), fa(), up_do()]
    part9 = [down_dot(double(ti())), down_dot(la()), up_fa(), down_dot(la()), down_dot(up_fa()), up_fa(), down_dot(up_so()), down_dot(fa())]
    part10 = list(map(down_dot, [half(up_so()), half(up_fa()), half(fa()), half(up_so()), up_fa(), half(la()), half(up_so()), half(up_fa()),
        half(la()), up_so(), half(ti()), half(up_la()), half(up_so()), half(ti()), half(up_la()), half(up_fa())]))
    part11 = part1
    part12 = part1
    part13 = part3[:-1] + [do()]
    part14 = [down_dot(double(up_la())), down_dot(ti()), ti(), re(), down_dot(ti()), ti(), up_do(), down_dot(up_la())]
    part15 = [down_dot(ti())] * 4 + [BL()] * 5

    seq1 = part1 + part2 + part3 + part4 + part5 + part6 + part7 + part8 + part9 + part10 + part11 + part12 + part13 + part14 + part15
    return seq1

def get_seq2():
    curr_oct = 4;
    # basic elements, "m" stands for "metre"
    m_0_up4_7   = [BL(), up_fa(curr_oct), ti(curr_oct)];
    m_0_2_up4   = [BL(), re(curr_oct), up_fa(curr_oct)];
    m_7_up1_2   = [ti(curr_oct), up_do(curr_oct + 1), re(curr_oct + 1)];
    m_0_3_7     = [BL(), mi(curr_oct), ti(curr_oct)];
    m_0_up1_5   = [BL(), up_do(curr_oct), so(curr_oct)];
    m_0_3_up4   = [BL(), mi(curr_oct), up_fa(curr_oct)];
    m_0_up1_up4 = [BL(), up_do(curr_oct), up_fa(curr_oct)];
    m_0_3_5     = [BL(), mi(curr_oct), so(curr_oct)];
    m_0_up3_7   = [BL(), fa(curr_oct), ti(curr_oct)]; # we treat #E as F
    m_0_up1_up5 = [BL(), up_do(curr_oct), up_so(curr_oct)];
    m_0_2_up4   = [BL(), re(curr_oct), up_fa(curr_oct)];
    m_0_1_5     = [BL(), do(curr_oct), so(curr_oct)];
    m_0_up3_up5 = [BL(), fa(curr_oct), up_so(curr_oct)];
    m_0_up1_2   = [BL(), up_do(curr_oct), re(curr_oct)];
    m_0_7_up1   = [BL(), ti(curr_oct - 1), up_do(curr_oct)];
    m_0_6_up1   = [BL(), la(curr_oct - 1), up_do(curr_oct)];
    m_0_7_2     = [BL(), ti(curr_oct - 1), re(curr_oct)];
    m_0_up1_3   = [BL(), up_do(curr_oct), mi(curr_oct)];

    # parts
    part1  = m_0_up4_7 + m_0_2_up4 +  m_7_up1_2;
    part2  = m_0_up4_7 + m_0_2_up4 + m_7_up1_2;
    part3  = m_0_3_7 + m_0_up4_7 + m_0_up1_5;
    part4  = m_0_2_up4 + m_0_3_up4 + m_0_2_up4;
    part5  = m_0_up1_up4 + m_0_2_up4 + m_0_3_5;
    part6  = m_0_up4_7 + m_0_2_up4 + m_7_up1_2;
    part7  = m_0_up4_7 + m_0_2_up4 + m_7_up1_2;
    part8  = m_0_up3_7 + m_0_up4_7 + m_0_up1_up5;
    part9  = m_0_2_up4 + m_0_up1_2 + m_0_7_up1;
    part10 = m_0_6_up1 + m_0_7_2 + m_0_up1_3;
    part11 = m_0_up4_7 + m_0_2_up4 + m_7_up1_2;
    part12 = m_0_up4_7 + m_0_2_up4 + m_7_up1_2;
    part13 = m_0_3_7 + m_0_up4_7 + m_0_1_5;
    part14 = m_0_2_up4 + m_0_up3_up5 + m_0_3_up4;
    part15 = [BL()] * 9;

    # seq
    seq2 = part1 + part2 + part3 + part4 + part5 + part6 + part7 + part8 + part9 + part10 + part11 + part12 + part13 + part14 + part15;
    return seq2;

def get_seq3():
    curr_oct = 3;
    curr_dur = 24;

    part1  = [ti(curr_oct, curr_dur * 9)]; # maybe it's better than [ti()] * 9;
    part2  = [la(curr_oct, curr_dur * 9)];
    part3  = [double(so(curr_oct)), BL(), double(up_fa(curr_oct)), BL(), double(mi(curr_oct)), BL()];
    part4  = [double(re(curr_oct)), BL(), double(up_do(curr_oct)), BL(), double(ti(curr_oct - 1)), BL()];
    part5  = [up_fa(curr_oct, curr_dur * 9)];
    part6  = part1;
    part7  = part2;
    part8  = [double(up_so(curr_oct)), BL(), double(up_fa(curr_oct)), BL(), double(fa(curr_oct)), BL()];
    part9  = [double(up_fa(curr_oct)), BL(), double(ti(curr_oct - 1)), BL(), double(up_do(curr_oct)), BL()];
    part10 = part5;
    part11 = part1;
    part12 = part2;
    part13 = part3;
    part14 = [double(re(curr_oct)), BL(), double(up_do(curr_oct)), BL(), double(up_fa(curr_oct)), BL()];
    part15 = [ti(curr_oct - 1, curr_dur * 4)] + [BL()] * 5;

    seq3 = part1 + part2 + part3 + part4 + part5 + part6 + part7 + part8 + part9 + part10 + part11 + part12 + part13 + part14 + part15;
    return seq3;

def get_seq4():
    curr_oct = 2;
    curr_dur = 24;

    part1  = [ti(curr_oct, curr_dur * 9)]; # maybe its better than [ti()] * 9;
    part2  = [la(curr_oct, curr_dur * 9)];
    part3  = [double(so(curr_oct)), BL(), double(up_fa(curr_oct)), BL(), double(mi(curr_oct)), BL()];
    part4  = [double(re(curr_oct)), BL(), double(up_do(curr_oct)), BL(), double(ti(curr_oct - 1)), BL()];
    part5  = [up_fa(curr_oct, curr_dur * 9)];
    part6  = part1;
    part7  = part2;
    part8  = [double(up_so(curr_oct)), BL(), double(up_fa(curr_oct)), BL(), double(fa(curr_oct)), BL()];
    part9  = [double(up_fa(curr_oct)), BL(), double(ti(curr_oct - 1)), BL(), double(up_do(curr_oct)), BL()];
    part10 = part5;
    part11 = part1;
    part12 = part2;
    part13 = part3;
    part14 = [double(re(curr_oct)), BL(), double(up_do(curr_oct)), BL(), double(up_fa(curr_oct)), BL()];
    part15 = [ti(curr_oct - 1, curr_dur * 4)] + [BL()] * 5;

    seq4 = part1 + part2 + part3 + part4 + part5 + part6 + part7 + part8 + part9 + part10 + part11 + part12 + part13 + part14 + part15;
    return seq4;


for key, item in Freqs_options.items():
    Freqs = item
    seq1 = get_seq1()
    doc.playNoteSeq(seq1, ScoreDraft.Piano())
    doc.mixDown("test_{}.wav".format(key))
