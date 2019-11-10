from random import *
import time
import timeit
def readfile(name,sep=''):          # 파일 읽기 함수
    file = open(name,'r')           # 지정한 이름의 파일을 읽기 모드로 엶
    str = file.readlines()          # 파일의 한 문장씩 str에 저장
    result=[]                       # 빈 리스트 생성
    for i in str:                   # 문자열에 저장된 단어를 i에 저장
        str2 = i.strip().split(sep) # sep 변수를 기준으로 단어를 분리하고 공백 삭제
        for j in str2:              # 위의 단어들을 순차적으로 j에 저장
            result.append(int(j))   # result 리스트에 j를 추가
    file.close()                    # 파일 닫기
    return result                   # 리스트 리턴
def writefile(list,name,sep):       # 파일을 저장하는 함수
    file = open(name,'w')           # 지정한 이름의 파일을 쓰기모드로 엶
    vstr = ''                       # 저장할 문자열
    for i in list:                  # 지정한 리스트의 요소들을 순차적으로 i에 저장
        vstr = vstr + str(i) + sep  # 지금까지의 문자열에 지정한 리스트의 요소들과 sep변수를 추가하여 저장
    vstr = vstr.rstrip(sep)         # 우측 공백 삭제
    file.writelines(vstr)           # 작성된 문자열을 한 문장으로 파일에 씀
    file.close()                    # 파일 종료
class compcount:        # 비교횟수를 카운트 할 클래스
    count = 0
    def __init__(self): # 생성 시 count를 0으로 초기화
        self.count = 0  
    def counts(self):   # counts 호출 시 count 하나 증가
        self.count+=1
    def reset(self):    # reset 호출 시 count 값 0으로 초기화
        self.count = 0
    def printc(self):   # printc 호출 시 비교 횟수 count 출력
        print("Comparison : " + str(self.count) + " times")
def insertionsort(A):           # insertion sort
    start = time.perf_counter()
    start_run = time.time()
    start_process = time.process_time()
    for j in range(1, len(A)):  # 1부터 리스트의 마지막까지 j에 저장
        key = A[j]              # 키값 = j번째 요소
        i = j - 1               # i = j-1
        while i >= 0 and A[i] > key:    # i가 0이상이고 해당 인덱스의 값이 키값보다 크면
            c.counts()
            A[i + 1] = A[i]     # 한 칸씩 요소를 뒤로 미룸
            i -= 1              # i = i-1
        c.counts()
        A[i + 1] = key          # i번째에 키값을 삽입
    print("Sorting Flow Time\t: " + str(time.perf_counter()-start))
    print("Sorting Run Time\t: " + str(time.time() - start_run))
    print("Sorting Process Time\t: " + str(time.process_time() - start_process))
def merge(A,p,q,r):
    n1 = q-p+1      # 왼쪽의 첫 인덱스
    n2 = r-q        # 오른쪽의 첫 인덱스
    L=[]
    R=[]
    for i in range(0,n1):
        L.append(A[p+i])    # 왼쪽 리스트 생성
    for j in range(0,n2):
        R.append(A[q+j+1])  # 오른쪽 리스트 생성
    i=0
    j=0
    L.append(999999999999999)   # 마지막 리스트는 infinity
    R.append(999999999999999)   # 마지막 리스트는 infinity
    for k in range(p,r+1):      # 처음부터 끝까지
        c.counts()
        if L[i] <= R[j]:        # 좌우 리스트의 값 중 작은 값을
            A[k] = L[i]         # 결과 리스트로 입력
            i += 1
        else:
            A[k] = R[j]         
            j += 1

def merge_sort(A,p,r):
    if p<r:
        q = (p+r)//2            # q는 리스트의 중앙
        merge_sort(A,p,q)
        merge_sort(A,q+1,r)
        merge(A,p,q,r)
def maxheapify(A,i,heapsize):
    c.counts()
    l = 2*i+1                   # i의 왼쪽 자식 노드
    r = 2*i+2                   # i의 오른쪽 자식 노드
    if l<heapsize and A[l]>A[i]:    # l이 힙사이즈보단 작으면서 i보다 크면
        largest = l                 # 가장 큰 값은 l
    else:
        largest = i                 # 아니면 가장 큰 값은 i
    if r<heapsize and A[r]>A[largest]:  # 위와 동일
        largest = r
    if largest != i:                        # i가 가장 큰 값이 아니면
        A[i],A[largest] = A[largest],A[i]   # 가장 큰값과 i 인덱스를 바꾼다.
        maxheapify(A,largest,heapsize)      # 가장 큰 값으로 maxheapify 실행
def buildmaxheap(A,heapsize):               
    for i in range(len(A)//2-1,-1,-1):      # 길이의 절반만큼 실행
        maxheapify(A,i,heapsize)            # maxheapify 반복 실행
def heapsort(A):
    start = time.perf_counter()
    start_run = time.time()
    start_process = time.process_time()
    heapsize = len(A)
    buildmaxheap(A,heapsize)
    for i in range(len(A)-1,0,-1):          # 리스트의 마지막부터 첫번째 인덱스까지 역순으로 실행
        A[0],A[i]=A[i],A[0]                 # i번째 요소와 첫번째 요소의 위치를 바꿔가면서
        heapsize -=1                        # heapsize를 하나 줄임
        maxheapify(A,0,heapsize)            # maxheapify 반복 실행
    print("Sorting Flow Time\t: " + str(time.perf_counter()-start))
    print("Sorting Run Time\t: " + str(time.time() - start_run))
    print("Sorting Process Time\t: " + str(time.process_time() - start_process))
def partition(A,p,r):                   # 파티션 함수
    x = A[r]                            # x(피벗)를 리스트의 마지막 인덱스
    i = p-1                             # i는 첫번째 인덱스 - 1
    for j in range(p,r):                # 리스트의 처음부터 끝까지
        c.counts()
        if A[j] <= x:                   # 해당 요소가 피벗보다 작거나 같으면
            i += 1                      # i + 1
            A[i],A[j] = A[j],A[i]       # 해당 요소와 자리 바꿈
    A[i+1],A[r] = A[r],A[i+1]           # i번쨰 원소와 마지막 원소 자리 바꿈
    return i+1                  
def quicksort(A,p,r):                   # 알고리즘은 수업시간에 배웠으므로 생략
    if p<r:
        q = partition(A,p,r)
        quicksort(A,p,q-1)
        quicksort(A,q+1,r)
def randomized_partition(A,p,r):
    i = randint(p,r)
    A[r],A[i] = A[i],A[r]
    return partition(A,p,r)
def randomized_quicksort(A,p,r):
    if p<r:
        q = randomized_partition(A,p,r)
        randomized_quicksort(A,p,q-1)
        randomized_quicksort(A,q+1,r)
def median(a, b, c):
    if a > b:
        if b > c:           # a > b > c
            return b;       
        elif a>c:           # a > c > b
            return c;
        else:               # c > a > b
            return a;
    else:
        if a > c:           # b > a > c
            return a;
        elif b > c:         # b > c > a
            return c;
        else:               # c > b > a
            return b;
def median_partition(A,p,r):
    left = A[p]                # left는 리스트의 첫 값
    right = A[r]               # right는 리스트의 마지막 값
    center = A[(p+r)//2]       # center는 리스트의 중앙 값
    i = A.index(median(left,center,right))  # i는 위의 세 값 중 가운데 값을 인덱스로 갖는 요소
    A[r],A[i] = A[i],A[r]
    return partition(A,p,r)
def median_quicksort(A,p,r):
    if p<r:
        q = median_partition(A,p,r)
        median_quicksort(A,p,q-1)
        median_quicksort(A,q+1,r)
sorted_32 = readfile('sorted32.txt',',')
random_32 = readfile('random32.txt',',')
random_1024 = readfile('random1024.txt',',')
list_name = [sorted_32, random_32, random_1024]
c = compcount()
i=0
for A in list_name:
    i+=1
    c.reset()
    #start = time.perf_counter()
    #start_run = time.time()
    #start_process = time.process_time()
    insertionsort(A)
    c.printc()
    #print("Sorting Flow Time\t: " + str(time.perf_counter()-start))
    #print("Sorting Run Time\t: " + str(time.time() - start_run))
    #print("Sorting Process Time\t: " + str(time.process_time() - start_process))
    #if i==1:
    #    vstr = 'insertion_sorted32.txt'
    #elif i==2:
    #    vstr = 'insertion_random32.txt'
    #else:
    #    vstr = 'insertion_random1024.txt'
    #writefile(A,vstr,',')
for A in list_name:
    i = 0
    while i < len(A)-2:
        if A[i]>A[i+1]:                     # 다음 값보다 이전 값이 더 크면 -> 정렬이 되어 있지 않음
            print(i)                        # 정렬되지 않은 값
            print(i+1)                      # 두 수를 출력
            print("Array is not Sorted!!!") # 정렬되지 않음을 출력
            break                           # 정렬되지 않은 값이 하나라도 있다면 정렬 실패이므로 다음은 무시
        i+=1

#insertionsort(A)
#merge_sort(A,0,len(A)-1)
#heapsort(A)
#median_quicksort(A,0,len(A)-1)
#randomized_quicksort(A,0,len(A)-1)
#quicksort(A,0,len(A)-1)