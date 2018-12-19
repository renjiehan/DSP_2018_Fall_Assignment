# coding=Big5

def main():
    word = []
    ZY_meta = []

# extract word and Zhu-Yin in meta state
    with open('Big5-ZhuYin.map', 'rb') as infile:
        for line in infile:
            data = line.translate(None, "[]'").split()
            word.append(data[0])
            ZY_meta.append(data[1])

# extract polymorphene of each word
    ZY = []
    for item in ZY_meta:
        data = ''.join(item).split('/')

        list_meta = []
        for i in range(len(data)):
            meta = data[i][:2]
            if(meta in list_meta):
                continue
            else:
                list_meta.append(meta)

        ZY.append((list_meta))
        list_meta = []

# put Zhu-Yin and corresponding word to dict
    table = {}
    for i in range(len(ZY)):
        for item in ZY[i]:
            if item  in table:
                table[item].append( word[i])
            else:
                table[item] = [ word[i]]
# write out
    for key in table:
        print(key),
        for val in table[key]:
            print(val),
        print ('')
        for val in table[key]:
            print val, val

if __name__ == '__main__':
    main()
