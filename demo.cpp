#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

// 1. 数组输入
vector<int> arrayInput() {
    int n;
    cout << "请输入数组元素个数: ";
    cin >> n;
    vector<int> arr(n);
    cout << "请输入 " << n << " 个数组元素: ";
    for (int i = 0; i < n; ++i) {
        cin >> arr[i];
    }
    return arr;
}

// 2. 数组排序
void arraySort(vector<int>& arr) {
    sort(arr.begin(), arr.end());
}

// 3. 元素插入
void elementInsert(vector<int>& arr) {
    int element, position;
    cout << "请输入要插入的元素: ";
    cin >> element;
    cout << "请输入要插入的位置 (0-based index): ";
    cin >> position;
    if (position >= 0 && position <= arr.size()) {
        arr.insert(arr.begin() + position, element);
    } else {
        cout << "插入位置无效!" << endl;
    }
}

// 4. 元素查找
void elementFind(const vector<int>& arr) {
    int element;
    cout << "请输入要查找的元素: ";
    cin >> element;
    auto it = find(arr.begin(), arr.end(), element);
    if (it != arr.end()) {
        cout << "元素 " << element << " 在数组中的位置: " << distance(arr.begin(), it) << endl;
    } else {
        cout << "元素 " << element << " 未找到!" << endl;
    }
}

// 5. 元素删除
void elementDelete(vector<int>& arr) {
    int position;
    cout << "请输入要删除的元素位置 (0-based index): ";
    cin >> position;
    if (position >= 0 && position < arr.size()) {
        arr.erase(arr.begin() + position);
    } else {
        cout << "删除位置无效!" << endl;
    }
}

// 6. 数组输出
void arrayOutput(const vector<int>& arr) {
    cout << "数组元素: ";
    for (int i = 0; i < arr.size(); ++i) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

// 7. 输出指定位置元素
void elementAtPosition(const vector<int>& arr) {
    int position;
    cout << "请输入要输出元素的位置 (0-based index): ";
    cin >> position;
    if (position >= 0 && position < arr.size()) {
        cout << "位置 " << position << " 的元素: " << arr[position] << endl;
    } else {
        cout << "位置无效!" << endl;
    }
}

// 8. 对指定个数的数组元素求和
void arraySum(const vector<int>& arr) {
    int count;
    cout << "请输入要求和的元素个数: ";
    cin >> count;
    if (count >= 0 && count <= arr.size()) {
        int sum = accumulate(arr.begin(), arr.begin() + count, 0);
        cout << "前 " << count << " 个元素的和: " << sum << endl;
    } else {
        cout << "元素个数无效!" << endl;
    }
}

int main() {
    vector<int> myArray;
    int choice;

    do {
        cout << "数组功能演示菜单:" << endl;
        cout << "1. 数组输入" << endl;
        cout << "2. 数组排序" << endl;
        cout << "3. 元素插入" << endl;
        cout << "4. 元素查找" << endl;
        cout << "5. 元素删除" << endl;
        cout << "6. 数组输出" << endl;
        cout << "7. 输出指定位置元素" << endl;
        cout << "8. 对指定个数的数组元素求和" << endl;
        cout << "0. 退出" << endl;
        cout << "请选择功能 (0-8): ";
        cin >> choice;

        switch (choice) {
            case 1:
                myArray = arrayInput();
                break;
            case 2:
                arraySort(myArray);
                cout << "数组已排序." << endl;
                break;
            case 3:
                elementInsert(myArray);
                break;
            case 4:
                elementFind(myArray);
                break;
            case 5:
                elementDelete(myArray);
                break;
            case 6:
                arrayOutput(myArray);
                break;
            case 7:
                elementAtPosition(myArray);
                break;
            case 8:
                arraySum(myArray);
                break;
            case 0:
                cout << "退出程序." << endl;
                break;
            default:
                cout << "无效的选择，请重新输入!" << endl;
        }
    } while (choice != 0);

    return 0;
}
