#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define MAX_STUDENTS 50
#define MAX_CLASSES 40

struct Student
{
    int roll;
    char name[30];
    char attendance[MAX_CLASSES];
    int joinedAtClass;
};

struct Student students[MAX_STUDENTS];
int studentCount = 0;
int classCount = 0;

// Find student using roll number
int findStudent(int roll)
{
    for (int i = 0; i < studentCount; i++)
    {
        if (students[i].roll == roll)
            return i;
    }
    return -1;
}

// Calculate eligible days
int eligibleDays(int index)
{
    return classCount - students[index].joinedAtClass;
}

// Count days present
int countPresent(int index)
{
    int present = 0;

    for (int i = students[index].joinedAtClass;
         i < classCount; i++)
    {
        if (students[index].attendance[i] == 'P')
            present++;
    }

    return present;
}

// Count days absent
int countAbsent(int index)
{
    return eligibleDays(index) - countPresent(index);
}

// Calculate attendance percentage
double calculatePercentage(int index)
{
    int total = eligibleDays(index);

    if (total == 0)
        return 0;

    return countPresent(index) * 100.0 / total;
}

// Calculate extra days needed
int daysNeeded(int index, double threshold)
{
    int present = countPresent(index);
    int total = eligibleDays(index);

    if (total == 0 ||
        calculatePercentage(index) >= threshold)
        return 0;

    if (threshold >= 100)
        return -1;

    return (int)ceil(
        (threshold * total - 100.0 * present) / (100.0 - threshold));
}

// Add a new student
void addStudent(void)
{
    int roll;

    if (studentCount >= MAX_STUDENTS)
    {
        printf("Student list is full!\n");
        return;
    }

    printf("Enter roll number: ");
    if (scanf("%d", &roll) != 1)
    {
        printf("Invalid input!\n");
        while (getchar() != '\n')
            ;
        return;
    }

    if (roll <= 0 || findStudent(roll) != -1)
    {
        printf("Invalid or duplicate roll number!\n");
        return;
    }

    students[studentCount].roll = roll;

    printf("Enter student name: ");
    scanf(" %29[^\n]", students[studentCount].name);

    // Ignore classes held before joining
    students[studentCount].joinedAtClass = classCount;

    studentCount++;

    printf("Student added successfully!\n");
}

// Take attendance
void takeAttendance(void)
{
    char status;

    if (studentCount == 0)
    {
        printf("Add students first!\n");
        return;
    }

    if (classCount >= MAX_CLASSES)
    {
        printf("Maximum classes reached!\n");
        return;
    }

    printf("\nAttendance for Day %d\n", classCount + 1);

    for (int i = 0; i < studentCount; i++)
    {
        do
        {
            printf("%s (P/A): ", students[i].name);

            if (scanf(" %c", &status) != 1)
                return;

            status = toupper((unsigned char)status);

            if (status != 'P' && status != 'A')
                printf("Enter P or A only!\n");

        } while (status != 'P' && status != 'A');

        students[i].attendance[classCount] = status;
    }

    classCount++;
    printf("Attendance recorded successfully!\n");
}

// Display attendance report
void viewReport(double threshold)
{
    if (studentCount == 0)
    {
        printf("No students available!\n");
        return;
    }

    printf("\n========== ATTENDANCE REPORT ==========\n");
    printf("ROLL\tNAME\t\tPRESENT\tABSENT\tPERCENT\tSTATUS\n");

    for (int i = 0; i < studentCount; i++)
    {
        int total = eligibleDays(i);
        int present = countPresent(i);
        int absent = countAbsent(i);

        if (total == 0)
        {
            printf("%d\t%-15s\t0\t0\tN/A\tNOT STARTED\n",
                   students[i].roll, students[i].name);
            continue;
        }

        double percentage = calculatePercentage(i);

        printf("%d\t%-15s\t%d\t%d\t%.2f%%\t%s\n",
               students[i].roll,
               students[i].name,
               present,
               absent,
               percentage,
               percentage >= threshold ? "OK" : "SHORTAGE");
    }
}

// Show students below attendance threshold
void showDefaulters(double threshold)
{
    int found = 0;

    printf("\n========== DEFAULTER LIST ==========\n");

    for (int i = 0; i < studentCount; i++)
    {
        if (eligibleDays(i) == 0)
            continue;

        if (calculatePercentage(i) < threshold)
        {
            int need = daysNeeded(i, threshold);

            printf("\nRoll: %d\n", students[i].roll);
            printf("Name: %s\n", students[i].name);
            printf("Days Present: %d\n", countPresent(i));
            printf("Days Absent: %d\n", countAbsent(i));
            printf("Attendance: %.2f%%\n",
                   calculatePercentage(i));

            if (need == -1)
                printf("Cannot reach 100%% now.\n");
            else
                printf("Must attend next %d days.\n", need);

            found++;
        }
    }

    if (found == 0)
        printf("There are no defaulters!\n");
}

// Search student
void searchStudent(double threshold)
{
    int roll;

    printf("Enter roll number: ");
    if (scanf("%d", &roll) != 1)
    {
        printf("Invalid input!\n");
        while (getchar() != '\n')
            ;
        return;
    }

    int index = findStudent(roll);

    if (index == -1)
    {
        printf("Student not found!\n");
        return;
    }

    printf("\nName: %s\n", students[index].name);
    printf("Roll Number: %d\n", students[index].roll);

    if (eligibleDays(index) == 0)
    {
        printf("No attendance recorded yet.\n");
        return;
    }

    printf("Days Present: %d\n", countPresent(index));
    printf("Days Absent: %d\n", countAbsent(index));
    printf("Total Days: %d\n", eligibleDays(index));

    printf("Attendance: %.2f%%\n",
           calculatePercentage(index));

    printf("Attendance History: ");

    for (int i = students[index].joinedAtClass;
         i < classCount; i++)
    {
        printf("%c ", students[index].attendance[i]);
    }

    printf("\nStatus: %s\n",
           calculatePercentage(index) >= threshold
               ? "OK"
               : "SHORTAGE");

    if (calculatePercentage(index) < threshold)
    {
        int need = daysNeeded(index, threshold);

        if (need == -1)
            printf("Cannot reach 100%% now.\n");
        else
            printf("Attend next %d days continuously.\n", need);
    }
}

// Class statistics
void classStatistics(void)
{
    double total = 0, highest = -1, lowest = 101;
    int highIndex = -1, lowIndex = -1, count = 0;

    if (studentCount == 0)
    {
        printf("No students available!\n");
        return;
    }

    for (int i = 0; i < studentCount; i++)
    {
        if (eligibleDays(i) == 0)
            continue;

        double percentage = calculatePercentage(i);

        total += percentage;
        count++;

        if (percentage > highest)
        {
            highest = percentage;
            highIndex = i;
        }

        if (percentage < lowest)
        {
            lowest = percentage;
            lowIndex = i;
        }
    }

    printf("\n========== CLASS STATISTICS ==========\n");
    printf("Total Students: %d\n", studentCount);
    printf("Total Days: %d\n", classCount);

    if (count == 0)
    {
        printf("No attendance data available.\n");
        return;
    }

    printf("Average Attendance: %.2f%%\n", total / count);
    printf("Highest Attendance: %.2f%% (%s)\n",
           highest, students[highIndex].name);
    printf("Lowest Attendance: %.2f%% (%s)\n",
           lowest, students[lowIndex].name);
}

// Main function
int main(void)
{
    int choice;
    double threshold = 75.0;

    do
    {
        printf("\n=================================\n");
        printf("   STUDENT ATTENDANCE ANALYZER\n");
        printf("=================================\n");

        printf("1. Add Student\n");
        printf("2. Take Attendance\n");
        printf("3. View Attendance Report\n");
        printf("4. Show Defaulters\n");
        printf("5. Search Student\n");
        printf("6. Class Statistics\n");
        printf("7. Change Attendance Threshold\n");
        printf("0. Exit\n");

        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input!\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        switch (choice)
        {
        case 1:
            addStudent();
            break;

        case 2:
            takeAttendance();
            break;

        case 3:
            viewReport(threshold);
            break;

        case 4:
            showDefaulters(threshold);
            break;

        case 5:
            searchStudent(threshold);
            break;

        case 6:
            classStatistics();
            break;

        case 7:
            printf("Enter threshold (1-100): ");

            if (scanf("%lf", &threshold) != 1)
            {
                printf("Invalid input!\n");
                threshold = 75.0;
                while (getchar() != '\n')
                    ;
                break;
            }

            if (threshold < 1 || threshold > 100)
            {
                printf("Invalid threshold!\n");
                threshold = 75.0;
            }
            break;

        case 0:
            printf("Thank you! Goodbye.\n");
            break;

        default:
            printf("Invalid choice!\n");
        }

    } while (choice != 0);

    return 0;
}