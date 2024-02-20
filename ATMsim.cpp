#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>

bool isFileExist(std::string path)
{
    std::fstream file(path);

    if (file.is_open())
    {
        file.close();
        return true;
    }
    file.close();
    return false;
}

void readFromBinary(std::string path, std::vector<int> &data)
{
    std::ifstream fileReader(path, std::ios::out | std::ios::binary);

    if (!fileReader)
    {
        std::cout << "Cannot open the file!";
        return;
    }
    fileReader.seekg(0);
    for (int i = 0; i < 6; i++)
    {
        fileReader.read((char *)&data[i], sizeof(int));
    }
    fileReader.close();

    if (!fileReader.good())
    {
        std::cout << "Error occured at reading time!";
    }
    return;
}

void writeToBinary(std::string path, std::vector<int> &data)
{
    std::ofstream file(path, std::ios::out | std::ios::binary);

    if (!file)
    {
        std::cout << "Cannot open the file!";
        return;
    }

    file.seekp(0);

    for (int i = 0; i < 6; i++)
    {
        file.write((char *)&data[i], sizeof(int));
    }

    file.close();

    if (!file.good())
    {
        std::cout << "Error occured at writing time!";
    }
    return;
}

int findAmount(std::vector<int> &vec)
{
    int bills = 0;
    for (int i = 0; i < vec.size(); i++)
    {
        bills += vec[i];
    }
    return bills;
}

int initialization(std::string path, std::vector<int> &data)
{
    const int READ_FILE = 0;
    const int NEW_FILE = 1;

    if (isFileExist(path))
    {
        return READ_FILE;
    }
    return NEW_FILE;
}

int findSum(std::vector<int> &bill, std::vector<int> &nominal)
{
    int sum = 0;
    for (int i = 0; i < bill.size(); i++)
    {
        sum += bill[i] * nominal[i];
    }
    return sum;
}

void showInfo(std::vector<int> &data)
{
    std::vector<int> nominal = {5000, 2000, 1000, 500, 200, 100};

    std::cout << "Bills: " << std::endl;
    for (int i = 0; i < data.size(); i++)
    {
        std::cout << data[i] << std::endl;
    }
    std::cout << "Total sum is: " << findSum(data, nominal) << std::endl;
}

void adding(std::vector<int> &bills)
{
    int billCount = findAmount(bills);
    int maxBills = 1000 - billCount;
    int billsToAdd = 0;
    for (int i = 0; i < bills.size() - 1; i++)
    {
        billsToAdd = std::rand() % (maxBills + 1);
        bills[i] += billsToAdd;
        maxBills -= billsToAdd;
    }
    bills[bills.size() - 1] += maxBills;
}

bool isMachineReady(std::string path, std::vector<int> &data)
{
    const bool SUCCESS = true;
    const bool FAILURE = false;

    int initFlag = initialization(path, data);

    const int NEW_FILE = 1;

    if (initFlag == NEW_FILE)
    {
        adding(data);
        writeToBinary(path, data);
        showInfo(data);
    }
    else
    {
        readFromBinary(path, data);
        showInfo(data);
    }
    return SUCCESS;
}

void withdrawing(std::vector<int> &bills, std::vector<int> &nominals, int &withdraw)
{
    for (int i = 0; i < nominals.size(); i++)
    {
        int billsToDraw = withdraw / nominals[i];
        if (bills[i] < billsToDraw)
        {
            billsToDraw = bills[i];
        }
        bills[i] -= billsToDraw;
        withdraw -= nominals[i] * billsToDraw;
    }

    if (withdraw > 0)
    {
        std::cout << "The ATM doesn't have needed money" << std::endl;
        return;
    }
    writeToBinary("balance.bin", bills);
}

int main()
{
    std::string path = "balance.bin";
    std::fstream atmBills;

    atmBills.open(path, std::fstream::binary | std::fstream::in | std::fstream::out);
    std::vector<int> nominals = {5000, 2000, 1000, 500, 200, 100};
    std::vector<int> bills = {0, 0, 0, 0, 0, 0};
    std::srand(std::time(nullptr));

    if (!isMachineReady(path, bills))
    {
        return 12;
    }

    std::cout << "Enter a command: ";
    char command;
    std::cin >> command;
    while (command != '0')
    {
        int billCount = findAmount(bills);
        if (billCount >= 1000)
        {
            std::cout << "The atm is full" << std::endl;
        }
        else if (command == '+' && billCount < 1000)
        {
            adding(bills);
            writeToBinary(path, bills);
            showInfo(bills);
        }

        if (command == '-')
        {
            std::cout << "How much money do you want to withdraw?" << std::endl;
            int withdraw = 0;
            std::cin >> withdraw;

            if (withdraw % 100 != 0)
            {
                std::cout << "The amount must be a multiple of 100!";
                return 1;
            }
            withdrawing(bills, nominals, withdraw);
            showInfo(bills);
        }
        std::cout << "Enter a command: ";
        std::cin >> command;
    }
    atmBills.close();
}