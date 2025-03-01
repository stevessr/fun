#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <ctype.h>

void normal_mode();
void game_mode();

#define MAX_RANGE 1000
#define LIFE_MAX 3

// 颜文字库
const char *emoticons[] = {
    "(≧ω≦)/", "(；´д｀)ゞ", "ヽ(￣д￣;)ノ",
    "(╯‵□′)╯︵┻━┻", "(´∀`)♪", "Σ( ° △ °|||)︴"};

typedef struct
{
    int num1, num2, num3;
    char op1, op2;
    int is_two_ops;
} Question;

// 工具函数声明
void splash_screen();
int get_random(int min, int max);
void generate_single_question(Question *q);
void generate_mixed_question(Question *q);
int calculate_answer(Question q);
void game_over_animation(int score);
void correct_animation();
void wrong_animation();

int main()
{
    system("CLS");
    srand(time(NULL));
    splash_screen();

    // 主循环
    while (1)
    {
        system("CLS");
        printf("\n  ～运算修炼道场～\n");
        printf("═══════════════════\n");
        printf(" 1. 普通修行模式\n");
        printf(" 2. 极限试炼模式\n");
        printf(" 3. 退出修炼\n");
        printf("═══════════════════\n");
        printf("选择修行方式：");

        int choice;
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            normal_mode();
            break;
        case 2:
            game_mode();
            break;
        case 3:
            printf("\n 哼，这就放弃了？\n");
            printf(" 下次可要更努力呀～%s\n", emoticons[4]);
            exit(0);
        default:
            printf("\n 输入错误！%s\n", emoticons[3]);
            getch();
        }
    }
    return 0;
}

void normal_mode()
{
    system("CLS");
    int correct = 0, total = 0;
    Question current;

    printf("\n 普通修行模式启动！%s\n", emoticons[0]);
    printf("════════════════════════\n");

    while (1)
    {
        // 随机生成混合或单题
        if (rand() % 2)
            generate_mixed_question(&current);
        else
            generate_single_question(&current);

        // 显示题目
        printf("\n 【第%d题】", total + 1);
        if (current.is_two_ops)
            printf(" %d %c %d %c %d = ", current.num1, current.op1,
                   current.num2, current.op2, current.num3);
        else
            printf(" %d %c %d = ", current.num1, current.op1, current.num2);

        // 获取答案
        int user_ans, correct_ans = calculate_answer(current);
        scanf("%d", &user_ans);

        // 判断结果
        total++;
        if (user_ans == correct_ans)
        {
            correct++;
            correct_animation();
        }
        else
        {
            wrong_animation();
            printf(" 正确答案是：%d %s\n", correct_ans, emoticons[1]);
        }

        // 继续判断
        printf("\n 继续修炼？(Y/N): ");
        if (toupper(getch()) != 'Y')
            break;
    }

    // 显示统计
    printf("\n\n 最终正确率：%.1f%% %s\n",
           (float)correct / total * 100, emoticons[4]);
    getch();
}

void game_mode()
{
    system("CLS");
    int lives = LIFE_MAX, score = 0, combo = 0;
    time_t start_time = time(NULL);
    Question current;

    printf("\n 极·限·试·炼·模·式！%s\n", emoticons[5]);
    printf("════════════════════════\n");
    printf(" 生命值：❤️❤️❤️\n");
    printf(" 倒计时：60秒\n");
    printf("════════════════════════\n");
    printf(" 准备就绪？(按下任意键开始)");
    getch();

    while (lives > 0 && difftime(time(NULL), start_time) < 60)
    {
        system("CLS");
        printf("\n 剩余时间：%d秒", 60 - (int)difftime(time(NULL), start_time));
        printf(" 生命：");
        for (int i = 0; i < lives; i++)
            printf("❤️");
        printf(" 连击：%d 得分：%d\n", combo, score);

        // 生成高难度题目
        generate_mixed_question(&current);

        // 动态题目难度
        if (combo > 5)
            current.op1 = (rand() % 2) ? '*' : '/';

        // 显示题目
        printf("\n ");
        if (current.is_two_ops)
            printf("%d %c %d %c %d = ", current.num1, current.op1,
                   current.num2, current.op2, current.num3);
        else
            printf("%d %c %d = ", current.num1, current.op1, current.num2);

        // 限时输入
        time_t question_start = time(NULL);
        int user_ans, correct_ans = calculate_answer(current);
        scanf("%d", &user_ans);

        // 判断结果
        if (user_ans == correct_ans)
        {
            combo++;
            score += 10 * combo;
            correct_animation();
        }
        else
        {
            lives--;
            combo = 0;
            wrong_animation();
            printf(" 正确答案：%d %s\n", correct_ans, emoticons[1]);
            if (lives > 0)
            {
                printf(" 还剩%d次机会！%s", lives, emoticons[2]);
                getch();
            }
        }

        // 时间惩罚
        if (difftime(time(NULL), question_start) > 5)
        {
            printf(" 太慢了！%s\n", emoticons[3]);
            lives--;
        }
    }

    game_over_animation(score);
}

// 题目的灵魂生成器（核心逻辑）
void generate_mixed_question(Question *q)
{
    q->is_two_ops = 1;
    int base_op = rand() % 4;

    // 根据运算符组合生成数值
    do
    {
        q->op1 = "+-*/"[rand() % 4];
        q->op2 = "+-*/"[rand() % 4];

        // 保证运算复杂度
        if (base_op < 2)
        { // 优先生成加减组合
            q->num1 = get_random(300, MAX_RANGE);
            q->num2 = get_random(100, q->num1);
            q->num3 = get_random(50, q->num2);
        }
        else
        { // 生成含乘除的组合
            q->num2 = get_random(1, 50);
            q->num3 = get_random(1, 20);
            q->num1 = get_random(100, MAX_RANGE);
        }

        // 处理除法约束
        if (q->op1 == '/')
            q->num2 = q->num1 * (get_random(1, 10));
        if (q->op2 == '/')
            q->num3 = q->num2 * (get_random(1, 5));
    } while (calculate_answer(*q) < 0 || calculate_answer(*q) > MAX_RANGE * 2);
}

// 更多实现细节...
// （由于篇幅限制，完整代码需展开实现运算符优先级处理、动画效果、异常处理等模块）
