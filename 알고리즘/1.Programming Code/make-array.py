from random import *
def writefile(list,name,sep):       # 파일을 저장하는 함수
    file = open(name,'w')           # 지정한 이름의 파일을 쓰기모드로 엶
    vstr = ''                       # 저장할 문자열
    for i in list:                  # 지정한 리스트의 요소들을 순차적으로 i에 저장
        vstr = vstr + str(i) + sep  # 지금까지의 문자열에 지정한 리스트의 요소들과 sep변수를 추가하여 저장
    vstr = vstr.rstrip(sep)         # 우측 공백 삭제
    file.writelines(vstr)           # 작성된 문자열을 한 문장으로 파일에 씀
    file.close()                    # 파일 종료



def insertionsort(A):           # 삽입 정렬
    for j in range(1, len(A)):
        key = A[j]
        i = j - 1
        while i >= 0 and A[i] > key:
            A[i + 1] = A[i]
            i -= 1
        A[i + 1] = key
random_32=[]    # 빈 리스트 생성
random_1024=[]  # 빈 리스트 생성
sorted_32=[]    # 빈 리스트 생성
for i in range(0,1024):     # 1024번 
    a = randint(1,10000)    # 1~9999 사이의 난수 
    while a in random_1024: # 생성된 난수가 리스트에 이미 존재하면
        a = randint(1,1000) # 난수 재생성
    random_1024.append(a)   # 리스트에 난수 추가
for i in range(0,32):       # 위와 동일
    a = randint(1,10000)
    while a in random_32:
        a = randint(1,10000)
    random_32.append(a)
for i in range(0,32):       # 위와 동일
    a = randint(1,10000)
    while a in sorted_32:
        a = randint(1,10000)
    sorted_32.append(a)

insertionsort(sorted_32)    # 정렬된 리스트 생성을 위한 정렬
writefile(random_32,'random32.txt',sep=',')     # 파일로 저장    
writefile(random_1024,'random1024.txt',sep=',') # 파일로 저장
writefile(sorted_32,'sorted32.txt',sep=',')     # 파일로 저장
