name = input()
bit  = int(input())

print(bit, end=" ")
for i in range(bit):
    print(name+"_"+str(i)+" ", end="")
