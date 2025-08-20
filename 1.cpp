#include <iostream>
#include <vector>
#include <string>

#define LE(x, y) ((x) <= (y))
#define MID3(x, y, z) (LE(nums[x], nums[y]) ? \
						(LE(nums[y], nums[z]) ? y : LE(nums[x], nums[z]) ? z : x) : \
						(LE(nums[z], nums[y]) ? y : LE(nums[z], nums[x]) ? z : x)) 

void	qsort(std::vector<int> & nums, const int start, const int end);
size_t	findPivotIndex(std::vector<int> & nums, const int start, const int end);
bool	isValidDigit(const std::string & str, size_t & i);

int main(void)  {
	std::string			str;
	size_t				strLen;
	std::vector<int>	nums;
	int					numsLen;

	// 0. input	rule 정의 
	std::cout << "정렬할 숫자들을 공백으로 구분하여 입력하세요.\n";
	std::cout << "부호 없는 양수만 입력값으로 처리합니다.\n";
	std::cout << "int 범위밖의 숫자 혹은 숫자외의 값이 들어올 경우(ex 알파벳) 프로그램이 종료됩니다.\n";
	std::cout << "모든 숫자들을 작성하신 후에는 Enter를 입력하세요.\n";
	std::cout << "예시: 6 5 5 7 7 8 10\n";

	// 1. input 유효성 검사 
	std::getline(std::cin, str);
	strLen = str.length();
	for (size_t i = 0; i < strLen; i++) {
		if (str[i] == ' ')
			continue;
		// 숫자외의 값이거나 int 범위 밖의 숫자일 경우 종료
		else if (str[i] < '0' || '9' < str[i] || !isValidDigit(str, i)) {
			std::cerr << "올바른 Input 값을 넣어주세요.\n";
			return (1);
		}
	}

	// 2. 문자열로 이루어진 숫자열을 int type으로 변환 후 vector에 삽입
	for (size_t i = 0; i < strLen; i++) {
		if ('0' <= str[i] && str[i] <= '9') {
			nums.push_back(atoi(str.c_str() + i));
			while ('0' <= str[i] && str[i] <= '9')
				i++;
		}
	}
	if (nums.size() == 0) {
		std::cerr << "하나 이상의 양수를 입력해주세요.\n";
		return (1);
	}

	// 3. quicksort 호출
	qsort(nums, 0, nums.size() - 1);
	
	// 4. 정렬된 리스트 출력
	std::cout << "오름차순으로 정렬된 리스트 출력\n";
	numsLen = nums.size();
	for (int i = 0; i < numsLen; i++) 
		std::cout << nums[i] << " ";
	std::cout << "\n";

	return (0);
}

void	qsort(std::vector<int> & nums, const int start, const int end) {
	int	pivot = findPivotIndex(nums, start, end);
	int	comp = start;

	// 0. degenerate case
	if (start >= end)
		return ;
	
	// 1. init
	if (pivot != end) {
		std::swap(nums[end], nums[pivot]);
		pivot = end;
	}

	// 2. sorting
	while (comp != pivot) {
		if (comp < pivot) {
			if (nums[comp] > nums[pivot]) {
				std::swap(nums[comp], nums[pivot]);
				std::swap(comp, pivot);
				comp--;
			}
			else
				comp++;
		}
		else { // pivot < comp 
			if (nums[pivot] > nums[comp]) {
				std::swap(nums[comp], nums[pivot]);
				std::swap(comp, pivot);
				comp++;
			}
			else
				comp--;
		}
	}

	// 3. recursive qsort
	qsort(nums, start, pivot - 1);
	qsort(nums, pivot + 1, end);
}

/* find pivot to recursive median of medians */
size_t	findPivotIndex(std::vector<int> & nums, const int start, const int end) {
	size_t	i0, i1, i2;
	size_t	totalSize = end - start + 1;

	if (totalSize <= 3) {
		if (totalSize <= 2)
			return (start);
		else // size == 3
			return (MID3(start, start + 1, end));
	}
	else {
		i0 = findPivotIndex(nums, start, start + totalSize / 3);
		i1 = findPivotIndex(nums, start + totalSize / 3 + 1, start + totalSize / 3 * 2);
		i2 = findPivotIndex(nums, start + totalSize / 3 * 2 + 1, end);
	}

	return (MID3(i0, i1, i2));
}

bool	isValidDigit(const std::string & str, size_t & i) {
	size_t	start = i;
	size_t	end;
	int		num;

	// 숫자 사이에 다른 값이 존재하는지 파악
	while (str[i] != ' ' && str[i] != '\0') {
		if ('0' <= str[i] && str[i] <= '9')
			i++;
		else
			return (false);
	}

	//  자리수가 10자이내인지 확인 (INT_MAX값이 10자리)
	end = i - 1;
	if (end - start + 1 > 10)
		return (false);

	// 0으로 시작하는지 확인 (ex 0001)
	if (end - start + 1 >= 1 && str[start] == '0')
		return (false);

	// int 범위 밖의 값인지 확인
	num = atoi(str.c_str() + start);
	if (num <= 0)
		return (false);
	while (num > 0) {
		if (num % 10 != str[end] - '0')
			return (false);
		num /= 10;
		end--;
	}

	return (true);
}
