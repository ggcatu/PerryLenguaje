def getPath(columnCount, rowCount, mat, x, y):
    if (x >= columnCount or y >= rowCount):
        return []
    down = getPath(columnCount, rowCount, mat, x+1, y)
    right = getPath(columnCount, rowCount, mat, x, y+1)
    paths = []
    if not down and not right:
        return [[mat[x][y]]]
    for d in down:
        paths.append( [mat[x][y]] + d)
    for r in right:
        paths.append( [mat[x][y]] + r)
    return paths

f = [[6, 1, 6],
     [4, 5, 7],
     [2, 3, 8]]

a = [[5, 1, 5, 6 ], [5, 3, 3, 5]]
r =  getPath(2, 4, a, 0, 0)
print(r)
if r:
    print(max(map(min, r)))
