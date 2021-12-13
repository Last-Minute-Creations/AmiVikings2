from PIL import Image, ImageDraw

HeightsIn = open('map1_heightmap.txt', 'r')
Lines = HeightsIn.readlines()

HeightsOut = []

MaxY = 0

for Line in Lines:
    try:
        Line = Line.strip().replace('  ', ' ')
        if Line == '':
            continue
        Parts = Line.split(' ')
        if Parts[1] != '->':
            # Parse error
            continue

        # Get Y
        Y = int(Parts[2]) + 1
        if Y > MaxY:
            MaxY = Y

        # Get X
        if Parts[0].find('..') != -1:
            # decode X range
            RangeText = Parts[0].split('..')
            StartX = int(RangeText[0])
            EndX = int(RangeText[1])
            for i in range(StartX, EndX + 1):
                HeightsOut.append([i, Y])
            pass
        else:
            # decode single X
            X = int(Parts[0])
            HeightsOut.append([X, Y])
            pass
    except:
        pass

print('read {} vals'.format(len(HeightsOut)))

ImgHeight = MaxY + 100
Img = Image.new('RGB', (len(HeightsOut), ImgHeight), color = 'black')

for Pair in HeightsOut:
    [x, y] = Pair
    Draw = ImageDraw.Draw(Img)
    Draw.rectangle([(x, y), (x, ImgHeight)], fill = 'red')

Img.save('heightmap.png')
