#include <iostream>
#include <Windows.h>

int main()
{
    MessageBoxW(nullptr, L"Hello", L"Hello", 0);
    std::cout << "Hello!\n";
}
