import random
import string
import subprocess
from textwrap import dedent

alnum = string.ascii_letters + string.digits

def trie(words, offset=0):
    cases = dict()
    for word in words:
        if offset == len(word):
            cases[''] = word
            continue
        ch = word[offset]
        if ch in cases:
            cases[ch] += [word]
        else:
            cases[ch] = [word]
    for ch, case in cases.items():
        if ch != '':
            cases[ch] = trie(cases[ch], offset+1)
    return cases

def trie_as_switch(words, t, offset=0, varname='word', term=lambda word: f'return {words.index(word)}'):
    indent = '  '*offset
    ret = f"{indent}switch ({varname}[{offset}]) " + "{\n"
    ret_end = f"{indent}default: break;\n{indent}" + "}\n"
    for ch, sub in t.items():
        if ch == '':
            assert(type(sub) is str)
            ret += f"{indent}case '\\0': {term(sub)};\n"
        else:
            ret += f"{indent}case '{ch}':\n"
            ret += f"{trie_as_switch(words, sub, offset+1, varname, term)}"
    return ret + ret_end

if __name__ == '__main__':
    words_len = 9
    words = set()
    while len(words) < words_len:
        length = random.randint(1, 5)
        word = ''
        while len(word) < length:
            word += random.choice(alnum)
        words.add(word)
    words = list(words)

    externs = dedent('''\
        // given a constexpr cindex, ...
        /*
        extern "C" std::size_t index(char const *word) {
            return cindex(word);
        }
        */

        ''') + "\n".join(map('extern "C" void extern_{}();'.format, words))

    with open('rekt.gen.cpp', 'w') as rekt_gen:
        rekt_gen.write(dedent('''\
            #include <rekt/trie.hpp>
            using namespace rekt;
            constexpr string_enum words = R"(
            '''))
        rekt_gen.write('\n'.join(words))
        rekt_gen.write(dedent('''\
            )";
            constexpr std::size_t cindex(char const *word) {
              using words_constant = string_enum::array_constant<words>;
              return trie<words_constant>::query(word);
            }
            '''))
        rekt_gen.write(externs)
        rekt_gen.write(dedent('''\
            // given a name, call the corresponding extern fn
            extern "C" void call_extern(char const *name) {
                switch (cindex(name)) {
                    %s
                }
            }
            ''' % "\n".join(map(lambda name: f'case cindex("{name}"): extern_{name}();', words))))

    words.sort()
    with open('hand.gen.cpp', 'w') as hand_gen:
        hand_gen.write("#include <limits>\n#include <utility>\n/*\n")
        hand_gen.write("\n".join(words))
        hand_gen.write("\n*/\n")
        hand_gen.write("constexpr std::size_t cindex(char const *word) {\n")
        t = trie(words)
        hand_gen.write(trie_as_switch(words, t))
        hand_gen.write("return std::numeric_limits<std::size_t>::max();\n}\n")
        hand_gen.write(externs)
        hand_gen.write(dedent('''\
            // given a name, call the corresponding extern fn
            extern "C" void call_extern(char const *name) {
                    %s
            }
            ''' % trie_as_switch(words, t, varname='name', term=lambda name: f'return extern_{name}()')))

    clang = 'clang++ -S -Iinclude -O3'.split()
    for method in 'hand rekt'.split():
        src = f'{method}.gen.cpp'
        subprocess.check_call(clang + [src])
        subprocess.check_call(clang + ['-emit-llvm', src])

