#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <sstream>
#include "caculator.h"
#include "ui.cpp"

using namespace std;

calulation c;
string temp = "";
string save = "";
string pass = "";
int mems;
int input = 0;
stringstream temps;
short ensure_int = 0;
short mode = 0;

void introduction()
{
    printline();
    print_line("��ӭ����Сѧ������ϰ��");
    print_line("��������C++����");
    print_line("���ܣ����Խ��мӡ������ˡ���������ϰ������������");
    print_line("��ϰ��������Ϸ�͡���ͨ��ϰ��");
}

void mode_get()
{
    short action = 1;
    auto action1 = [&action]()
    {
        printline();
        print_line("��ѡ����ϰģʽ��");
        print_line("1.��Ϸ��");
        print_line("2.��ͨ��ϰ��");
        input_line("���ѡ����(����0�˳���Ϸ)��");
        cin >> temp;
        action = 2;
    };
    auto action2 = [&action]()
    {
        while (temp != "1" && temp != "2" && temp != "0")
        {
            input_line("����������������룺");
            cin >> temp;
        }
        action = 3;
    };
    auto action3 = [&action]()
    {
        pass = "";
        input_line("��ȷ����?(����1ȷ��,2����)");
        cin >> pass;
        if (pass == "1")
        {
            action = 4;
        }
        else if (pass == "2")
        {
            action = 1;
        }
        else
        {
            while (pass != "1" && pass != "2")
            {
                input_line("����������������룺");
                cin >> pass;
            }
        }
    };
    auto action4 = [&action]()
    {
        if (temp == "1")
        {
            mode = 1;
            print_line("��Ϸģʽ�ѿ���");
        }
        else if (temp == "2")
        {
            mode = 0;
            print_line("��ͨ��ϰģʽ�ѿ���");
        }
        else
        {
            mode = -1;
        }
        action = 5;
    };
    while (action < 5)
    {
        switch (action)
        {
        case 1:
            action1();
            break;
        case 2:
            action2();
            break;
        case 3:
            action3();
            break;
        case 4:
            action4();
            break;
        default:
            action = 1;
            break;
        }
    }
}

void game()
{
    short action = 1;
    auto action1 = [&action]()
    {
        printline();
        print_line("���ѽ�����Ϸģʽ");
        action = 2;
    };

    double towait = 0;

    auto start = std::chrono::high_resolution_clock::now();
    cin >> towait;
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    auto action2 = [&action, &start, &end, &duration, &towait]()
    {
        input_line("������ÿ�������ʱ����λ:�룩��");
        start = std::chrono::high_resolution_clock::now();
        cin >> towait;
        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        action = 3;
    };

    auto action3 = [&action, &towait]()
    {
        while (towait == 0)
        {
            input_line("�����������������룺");
            cin >> towait;
        }
        action = 4;
    };

    auto action4 = [&action]()
    {
        input_line("��ȷ����?(����1ȷ��,2����)");
        cin >> pass;
        if (pass == "1")
        {
            action = 5;
        }
        else if (pass == "2")
        {
            action = 2;
        }
        else
        {
            while (pass != "1" && pass != "2")
            {
                input_line("����������������룺");
                cin >> pass;
            }
            if (pass == "1")
            {
                action = 5;
            }
            else if (pass == "2")
            {
                action = 2;
            }
        }
    };

    auto action5 = [&duration, &action]()
    {
        print_line("�޾����ؼ�����ʼ");
        waiting();
        printline();
        srand(static_cast<unsigned int>(duration) % 65535);
        action = 6;
    };

    long long a = 0;

    auto action6 = [&start, &end, &duration, &action]()
    {
        mems = 1;
        c.random();
        cout << "*";
        cout << c;
        cout << "=" << endl;
        input_line("�������(ȡ��������)��");
        start = std::chrono::high_resolution_clock::now();
        cin >> temp;
        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        cout << setw(consoleWidth - 1) << left << "*��ʱ: " + to_string(duration) + "����" << "*" << endl;
        temps = stringstream(temp);
        temps >> input;
        action = 7;
    };

    auto action7 = [&a, &duration, &towait, &action]()
    {
        if (input == c.get_result())
        {
            print_line("����ȷ");
            cout << "*";
            system("pause");
            clear_screen();
            action = 7;
        }
        else
        {
            print_line("�𰸴�����Ϸ��ֹ����ȷ��Ϊ" + to_string(int(c.result)));
            action = 8;
        }
        if (duration > 1000 * towait)
        {
            print_line("��ʱ����Ϸ��ֹ");
            action = 8;
        }
        printline();
        a++;
    };

    auto action8 = [&a, &action]()
    {
        print_line("�ۼƳɹ�" + to_string(a) + "��");
        after_finish();
        action = 9;
    };

    while (action < 9)
    {
        switch (action)
        {
        case 1:
            action1();
            break;
        case 2:
            action2();
            break;
        case 3:
            action3();
            break;
        case 4:
            action4();
            break;
        case 5:
            action5();
            break;
        case 6:
            action6();
            break;
        case 7:
            action7();
            break;
        case 8:
            action8();
            break;
        default:
            action = 1;
            break;
        }
    }
}

void pratise()
{
    short action = 1;
    auto action1 = [&action]()
    {
        printline();
        input_line("��ѡ������������Ŀ���ͣ�1���ӷ���2��������3���˷���4��������5����ϣ�q���˳�����");
        cin >> temp;
        action = 2;
    };

    auto action2 = [&action]()
    {
        while (temp != "5" && temp != "4" && temp != "3" && temp != "2" && temp != "1" && temp != "q")
        {
            input_line("�����������������:");
            cin >> temp;
        }
        action = 3;
    };

    auto action3 = [&action]()
    {
        input_line("��ȷ����?(����1ȷ��,2����)");
        cin >> pass;
        action = 4;
    };

    auto action4 = [&action]()
    {
        if (pass == "1")
        {
            action = 5;
        }
        else if (pass == "2")
        {
            if (ensure_int == 0)
            {
                action = 1;
            }
            else
            {
                action = 5;
            }
        }
        else
        {
            input_line("�����������������:");
            cin >> pass;
        }
    };
    auto action5_2 = [&action]()
    {
        if (temp == "4" || temp == "5" && ensure_int == 0)
        {
            string str;
            input_line("�Ƿ����������ף���֤����һ����������1���ǣ�2����:");
            cin >> str;
            if (str == "1")
            {
                ensure_int = 1;
            }
            else if (str == "2")
            {
                ensure_int = 2;
            }
            else
            {
                input_line("�����������������:");
                cin >> str;
            }
            action = 3;
        }
    };
    auto action5 = [&action, &action5_2]()
    {
        if (temp == "q")
        {
            mode = -1;
            action = 128;
            return;
        }
        action = 6;
    };

    long towait;
    long tars;
    auto start = std::chrono::high_resolution_clock::now();
    auto end = chrono::high_resolution_clock::now();

    auto action6 = [&action, &towait, &start, &end, &tars]()
    {
        input_line("���������⣺");
        start = std::chrono::high_resolution_clock::now();
        cin >> towait;
        end = chrono::high_resolution_clock::now();
        action = 7;
        tars = towait;
    };

    auto action7 = [&action]()
    {
        input_line("��ȷ����?(����1ȷ��,2����)");
        cin >> pass;
        action = 8;
    };

    auto action8 = [&action]()
    {
        if (pass == "1")
        {
            action = 9;
        }
        else if (pass == "2")
        {
            action = 6;
        }
        else
        {
            action = 8;
            input_line("�����������������:");
            cin >> pass;
        }
    };

    auto duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    long a = 0;
    vector<pair<string, int>> wrong, wrong2;
    wrong.clear();
    wrong2.clear();

    auto action9 = [&action, &duration, &end, &start]()
    {
        duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        waiting();
        srand(static_cast<unsigned int>(duration) % 65535);
        save = temp;
        action = 10;
    };

    auto action10 = [&action, &duration, &end, &start]()
    {
        printline();
        if (ensure_int == 1)
        {
            c.random(save);
        }
        else
        {
            c.random(save,2);
        }
        cout << "*";
        cout << c;
        cout << "=" << endl;
        input_line("�������(ȡ��������,����q�˳�)��");
        start = std::chrono::high_resolution_clock::now();
        cin >> temp;
        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        print_line("*��ʱ: " + to_string(duration) + "����");
        temps = stringstream(temp);
        temps >> input;
        action = 11;
    };

    auto action11 = [&a, &wrong, &action, &towait]()
    {
        if (temp == "q")
        {
            mode = -1;
            action = 128;
        }
        else if (input == c.get_result())
        {

            print_line("����ȷ");
            a++;
            action = 10;
            towait--;
            if (towait == 0)
            {
                action = 12;
            }
        }
        else
        {
            print_line("�𰸴�����ȷ��Ϊ" + to_string(int(c.result)));
            wrong.push_back({c.get_string(), int(c.result)});
            action = 12;
        }
    };

    auto action12 = [&a, &action, &tars, &wrong]()
    {
        printline();
        print_line("�ۼƳɹ�" + to_string(a) + "��");
        print_line("�ɹ���" + to_string(a * 1.0 / tars) + "%");
        print_line("�ܹ�" + to_string(tars) + "��");
        printline();
        if (wrong.size() != 0)
        {
            print_line("�����Ǵ���");
            for (auto &i : wrong)
            {
                print_line(i.first);
            }
        }
        action = 13;
    };

    auto action13 = [&action]()
    {
        printline();
        input_line("�Ƿ�Ҫ��ϰ���⣨1���ǣ�2���񣩣�");
        cin >> temp;
        action = 14;
    };

    auto action14 = [&action]()
    {
        if (temp == "1")
        {
            action = 15;
        }
        else if (temp == "2")
        {
            action = 128;
        }
        else
        {
            action = 14;
            input_line("�����������������:");
            cin >> temp;
        }
    };

    auto action15 = [&action, &wrong, &start, &end, &duration, &wrong2]()
    {
        clear_screen();
        for (int i = 0; i < wrong.size(); i++)
        {
            input_line(wrong[i].first);
            input_line("�������(ȡ��������,����q�˳�)��");
            start = std::chrono::high_resolution_clock::now();
            cin >> temp;
            end = chrono::high_resolution_clock::now();
            duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            print_line("*��ʱ: " + to_string(duration) + "����");
            temps = stringstream(temp);
            temps >> input;
            if (temp == "q")
            {
                mode = -1;
                action = 128;
                break;
            }
            else if (input == c.get_result())
            {

                print_line("����ȷ");
            }
            else
            {
                print_line("�𰸴�����ȷ��Ϊ" + to_string(wrong[i].second));
                wrong2.push_back({wrong[i].first, wrong[i].second});
            }
        }
        action = 16;
    };

    auto action16 = [&a, &action, &tars, &wrong2, &wrong]()
    {
        printline();
        if (wrong2.size() != 0)
        {
            print_line("������ʣ�µĴ���");
            for (auto &i : wrong2)
            {
                print_line(i.first);
            }
        }
        action = 13;
        wrong = wrong2;
    };

    while (action < 17)
    {
        switch (action)
        {
        case 1:
            action1();
            break;
        case 2:
            action2();
            break;
        case 3:
            action3();
            break;
        case 4:
            action4();
            break;
        case 5:
            action5();
            break;
        case 6:
            action6();
            break;
        case 7:
            action7();
            break;
        case 8:
            action8();
            break;
        case 9:
            action9();
            break;
        case 10:
            action10();
            break;
        case 11:
            action11();
            break;
        case 12:
            action12();
            break;
        case 13:
            action13();
            break;
        case 14:
            action14();
            break;
        case 15:
            action15();
            break;
        case 16:
            action16();
            break;

        default:
            action = 1;
            break;
        }
    }
    after_finish();
}

void action()
{

    while (mode != -1)
    {
        mode_get();
        clear_screen();
        switch (mode)
        {
        case 1:
            game();
            break;
        case 0:
            pratise();
            break;
        case -1:
            break;
        default:
            break;
        }
    }
}

void thanks()
{
    print_line("��л���ʹ��");
}
