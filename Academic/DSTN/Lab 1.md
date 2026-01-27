# Testcases

> [!success] 6/7

> [!failure] 1/7


## Test Case 1 : CLOOK

Expected: [(3, 0, 255, 30, 285), (1, 0, 270, 30, 300), (18, 40, 80, 30, 150), (17, 0, 300, 30, 330), (19, 0, 30, 30, 60), (34, 40, 20, 30, 90), (30, 0, 210, 30, 240)]

Actual: [(3, 0, 255, 30, 285, 0, 1), (1, 0, 270, 30, 300, 0, 1), (18, 40, 80, 30, 150, 1, 1), (17, 0, 300, 30, 330, 1, 1), (19, 0, 30, 30, 60, 1, 1), (34, 40, 20, 30, 90, 2, 1), (30, 0, 210, 30, 240, 2, 1)]

| block | track | direction |
| ----- | ----- | --------- |
| 3     | 0     | inward    |
| 1     | 0     | inward    |
| 18    | 1     | inward    |
| 17    | 1     | inward    |
| 19    | 1     | inward    |
| 34    | 2     | inward    |
| 30    | 2     | inward    |
> [!success] PASSED

## Test Case 2 : CLOOK

Expected: [(18, 40, 305, 30, 375), (13, 0, 180, 30, 210), (30, 40, 80, 30, 150), (1, 80, 100, 30, 210), (17, 40, 50, 30, 120), (16, 0, 300, 30, 330)]
Actual: [(18, 40, 305, 30, 375, 1, 1), (13, 0, 180, 30, 210, 1, 1), (30, 40, 80, 30, 150, 2, 1), (1, 80, 100, 30, 210, 0, 0), (17, 40, 50, 30, 120, 1, 1), (16, 0, 300, 30, 330, 1, 1)]

| block | track | direction |
| ----- | ----- | --------- |
| 18    | 1     | 1         |
| 13    | 1     | 1         |
| 30    | 2     | 1         |
| 1     | 0     | 0         |
| 17    | 1     | 1         |
| 16    | 1     | 1         |
> [!success] PASSED

## Test Case 3 : CLOOK

Expected: [(28, 0, 71, 7, 78), (23, 80, 55, 8, 143), (13, 0, 7, 8, 15), (1, 80, 2, 8, 90), (10, 0, 60, 7, 67), (8, 0, 68, 7, 75), (4, 0, 53, 7, 60), (50, 320, 18, 7, 345), (40, 80, 18, 7, 105), (36, 0, 53, 7, 60)]
Actual: [(28, 0, 71, 7, 78, 2, 0), (40, 80, 3, 7, 90, 3, 1), (36, 0, 53, 7, 60, 3, 1), (50, 80, 18, 7, 105, 4, 1), (1, 320, 25, 8, 353, 0, 0), (10, 0, 60, 7, 67, 0, 0), (8, 0, 68, 7, 75, 0, 0), (4, 0, 53, 7, 60, 0, 0), (23, 80, 55, 8, 143, 1, 1), (13, 0, 7, 8, 15, 1, 1)]

| block | track | direction |
| ----- | ----- | --------- |
| 28    | 2     | 0         |
| 40    | 3     | 1         |
| 36    | 3     | 1         |
| 50    | 4     | 1         |
| 1     | 0     | 0         |
| 10    | 0     | 0         |
| 8     | 0     | 0         |
| 4     | 0     | 0         |
| 23    | 1     | 1         |
| 13    | 1     | 1         |
> [!fail] FAILED

After using, self.initialDir
Expected: [(28, 0, 71, 7, 78), (23, 80, 55, 8, 143), (13, 0, 7, 8, 15), (1, 80, 2, 8, 90), (10, 0, 60, 7, 67), (8, 0, 68, 7, 75), (4, 0, 53, 7, 60), (50, 320, 18, 7, 345), (40, 80, 18, 7, 105), (36, 0, 53, 7, 60)]
Actual: [(28, 0, 71, 7, 78, 2, 0), (13, 80, 70, 8, 158, 1, 0), (23, 0, 67, 8, 75, 1, 0), (4, 80, 40, 7, 127, 0, 0), (8, 0, 23, 7, 30, 0, 0), (10, 0, 8, 7, 15, 0, 0), (1, 0, 15, 8, 23, 0, 0), (50, 320, 40, 7, 367, 4, 1), (36, 80, 78, 7, 165, 3, 0), (40, 0, 23, 7, 30, 3, 0)]

| block | track | direction |
| ----- | ----- | --------- |
| 28    | 2     | 0         |
| 13    | 1     | 0         |
| 23    | 1     | 0         |
| 4     | 0     | 0         |
| 8     | 0     | 0         |
| 10    | 0     | 0         |
| 1     | 0     | 0         |
| 50    | 4     | 1         |
| 36    | 3     | 0         |
| 40    | 3     | 0         |

> [!fail] FAILED

FINAL CODE THAT PASSED
```python
def DoCLOOK(self, rList):
	# TODO: Implement C-LOOK scheduling
	# This should return (block, index) tuple
	# rList = sorted(rList, key=lambda x: self.blockToTrackMap[x[0]])
	# return rList

	#* IMP Things ig
	#* 1. self.armTrack is the current track of the arm
	#* 2. self.currentDirection is the current direction of the arm (1 = inward, 0 = outward)
	#* Inward means higher number
	#* 3. we've block,index tuples. We need to sort by track number. So, we'll need to use the 
	#* blockToTrackMap to get the track number for each block.
	#* Sort all of them, by their track number, and then, depending on the direction, 
	#* we can pick the next one.
	#* 4. In case of ties (same track), preserve original request ordering
	#* 5. Remember to skip over requests that are already done (STATE_DONE)
	#* 6. self.initialDir is the initial direction of the arm (1 = inward, 0 = outward)

	sortedList = sorted([(block,index) for block,index in rList 
						 if self.requestState[index] != STATE_DONE] ,
						   key=lambda x: (self.blockToTrackMap[x[0]], x[1]))
	
	# def lower_bound(arr, curr):
	#     lo, hi = 0, len(arr)
	#     while lo < hi:
	#         mid = (lo + hi) // 2
	#         if self.blockToTrackMap[arr[mid][0]] < curr:
	#             lo = mid + 1
	#         else:
	#             hi = mid
	#     return lo
	# # def upper_bound(arr, curr):
	# #     lo, hi = 0, len(arr)
	# #     while lo < hi:
	# #         mid = (lo + hi) // 2
	# #         if self.blockToTrackMap[arr[mid][0]] <= curr:
	# #             lo = mid + 1
	# #         else:
	# #             hi = mid
	# #     return lo - 1

	
	# # if (self.currentDirection == 1):
	# t = lower_bound(sortedList, self.armTrack)
	# if (t < len(sortedList)):
	#     return sortedList[t]
	# else: return sortedList[0]
	# else:
	#     t = upper_bound(sortedList, self.armTrack)
	#     if (t >= 0):
	#         return sortedList[t]
	#     else: return sortedList[-1]
	
	# for req in sortedList:
	#     if self.blockToTrackMap[req[0]] >= self.armTrack:
	#         return req
		
	# return sortedList[0]

	if self.initialDir == 1: 
		for req in sortedList:
			if self.blockToTrackMap[req[0]] >= self.armTrack:
				return req
		return sortedList[0]
	else: 
		sortedList.sort(key=lambda x: (-self.blockToTrackMap[x[0]], x[1]))
		for req in sortedList:
			if self.blockToTrackMap[req[0]] <= self.armTrack:
				return req
		return sortedList[0]
```

| block | track | direction |
| ----- | ----- | --------- |
| 28    | 2     | 0         |
| 23    | 1     | 0         |
| 13    | 1     | 0         |
| 1     | 0     | 0         |
| 10    | 0     | 0         |
| 8     | 0     | 0         |
| 4     | 0     | 0         |
| 50    | 4     | 1         |
| 40    | 3     | 0         |
| 30    | 3     | 0         |
>[!success] PASSED

...Other Testcases of CLOOK have also passed

## Test_VR_Case_1 (V(R))

