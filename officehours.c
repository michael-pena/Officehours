/*
Michael Pena
ID: 1001073634
*/

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

/*** Constants that define parameters of the simulation ***/

#define MAX_SEATS 3        /* Number of seats in the professor's office */
#define professor_LIMIT 10 /* Number of students the professor can help before he needs a break */
#define MAX_STUDENTS 1000  /* Maximum number of students in the simulation */

#define CLASSA 0
#define CLASSB 1
#define CLASSC 2
#define CLASSD 3

/* TODO */
/* Add your synchronization variables here */

/* Basic information about simulation.  They are printed/checked at the end
 * and in assert statements during execution.
 *
 * You are responsible for maintaining the integrity of these variables in the
 * code that you develop.
 */

static int students_in_office;   /* Total numbers of students currently in the office */
static int classa_inoffice;      /* Total numbers of students from class A currently in the office */
static int classb_inoffice;      /* Total numbers of students from class B in the office */
static int classa_consectutive;  // this wil be used to check after 5 students from class a it will professor will switch to b students
static int classb_consectutive;  // this wil be used to check after 5 students from class b it will professor will switch to a students
static int students_since_break = 0;
static int classa_wait = 0;
static int classb_wait = 0;


typedef struct
{
  int arrival_time;  // time between the arrival of this student and the previous student
  int question_time; // time the student needs to spend with the professor
  int student_id;
  int class;
} student_info;

/* Called at beginning of simulation.
 * TODO: Create/initialize all synchronization
 * variables and other global variables that you add.
 */

 sem_t office_sema;
 pthread_mutex_t count_mutex;
 int count = 3;
 int sem_val;


 // Declaration of thread condition variable
pthread_cond_t classa_cv = PTHREAD_COND_INITIALIZER;
pthread_cond_t classb_cv = PTHREAD_COND_INITIALIZER;

pthread_mutex_t class_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t count_mutex;




static int initialize(student_info *si, char *filename)
{
  students_in_office = 0;
  classa_inoffice = 0;
  classb_inoffice = 0;
  students_since_break = 0;

  /* Initialize your synchronization variables (and
   * other variables you might use) here
   */
   sem_init(&office_sema, 0, count);


  /* Read in the data file and initialize the student array */
  FILE *fp;

  if((fp=fopen(filename, "r")) == NULL)
  {
    printf("Cannot open input file %s for reading.\n", filename);
    exit(1);
  }

  int i = 0;
  while ( (fscanf(fp, "%d%d%d\n", &(si[i].class), &(si[i].arrival_time), &(si[i].question_time))!=EOF) &&
           i < MAX_STUDENTS )
  {
    i++;
  }

 fclose(fp);
 return i;
}

/* Code executed by professor to simulate taking a break
 * You do not need to add anything here.
 */
static void take_break()
{
  printf("The professor is taking a break now.\n");
  sleep(5);
  assert( students_in_office == 0 );
  students_since_break = 0;
}

/* Code for the professor thread. This is fully implemented except for synchronization
 * with the students.  See the comments within the function for details.
 */
void *professorthread(void *junk)
{
  printf("The professor arrived and is starting his office hours\n");

  /* Loop while waiting for students to arrive. */
  while (1)
  {

    /* TODO */
    /* Add code here to handle the student's request.             */
    /* Currently the body of the loop is empty. There's           */
    /* no communication between professor and students, i.e. all  */
    /* students are admitted without regard of the number         */
    /* of available seats, which class a student is in,           */
    /* and whether the professor needs a break. You need to add   */
    /* all of this.                                               */

    pthread_mutex_lock(&count_mutex);

    while ( !(students_in_office == 0) )
    {
      if ( (students_since_break == 10) && (students_in_office == 0 ) )
      {
        take_break();
      }

    }

    pthread_mutex_unlock(&count_mutex);
    /*

    //if a class b student comes while there are class students
    if ( !(classa_inoffice == 0) && (classb_inoffice < 0 ))
    {
      //signal b to sleep until a students finish
      pthread_cond_wait();
    }
    */


  }
  pthread_exit(NULL);
}

/* Code executed by a class A student to enter the office.
 * You have to implement this.  Do not delete the assert() statements,
 * but feel free to add your own.
 */
void classa_enter()
{

  /* TODO */
  /* Request permission to enter the office.  You might also want to add  */
  /* synchronization for the simulations variables below                  */
  /* YOUR CODE HERE.                                                     */
  classa_consectutive += 1;

  if (classb_consectutive == 5)
  {
    classb_consectutive = 0;
  }

  //add while loop here
  pthread_mutex_lock(&count_mutex);


  while (classb_inoffice > 0 || classb_consectutive == 5) // if there are class b students in office this thread sleeps
  {
    pthread_cond_wait(&classa_cv, &class_mutex);
  }

  sem_wait(&office_sema);


  students_in_office += 1;
  students_since_break += 1;
  classa_inoffice += 1;

  pthread_mutex_unlock(&count_mutex);

}

/* Code executed by a class B student to enter the office.
 * You have to implement this.  Do not delete the assert() statements,
 * but feel free to add your own.
 */
void classb_enter()
{

  /* TODO */
  /* Request permission to enter the office.  You might also want to add  */
  /* synchronization for the simulations variables below                  */
  /*  YOUR CODE HERE.                                                     */
  classb_consectutive += 1;

  if (classa_consectutive == 5)
  {
    classb_consectutive = 0;
  }


  //add while loop herb
  pthread_mutex_lock(&count_mutex);


  while (classa_inoffice > 0 || classa_consectutive == 5) // if there are class a students in office this thread sleeps
  {

    pthread_cond_wait(&classb_cv, &class_mutex);
  }

  if (classb_consectutive == 5)
  {
    //pause thread and switch to classa_enter()
  }


  sem_wait(&office_sema);


  students_in_office += 1;
  students_since_break += 1;
  classb_inoffice += 1;

  pthread_mutex_unlock(&count_mutex);
}

/* Code executed by a student to simulate the time he spends in the office asking questions
 * You do not need to add anything here.
 */
static void ask_questions(int t)
{
  sleep(t);
}

/* Code executed by a class A student when leaving the office.
 * You need to implement this.  Do not delete the assert() statements,
 * but feel free to add as many of your own as you like.
 */
static void classa_leave()
{
  /*
   *  TODO
   *  YOUR CODE HERE.
   */
  sem_post(&office_sema);


  students_in_office -= 1;
  classa_inoffice -= 1;


  if ( (classa_inoffice == 0) && (students_in_office == 0) ) //if there are 0 class a students in the room, this wakes up waiting class b threads
  {
    pthread_cond_signal(&classb_cv);
  }


}

/* Code executed by a class B student when leaving the office.
 * You need to implement this.  Do not delete the assert() statements,
 * but feel free to add as many of your own as you like.
 */
static void classb_leave()
{
  /*
   * TODO
   * YOUR CODE HERE.
   */
   sem_post(&office_sema);


  students_in_office -= 1;
  classb_inoffice -= 1;


  if ( (classb_inoffice == 0) && (students_in_office == 0) ) //if there are 0 class b students in the room, this wakes up waiting class a threads
  {
    pthread_cond_signal(&classa_cv);
  }

}

/* Main code for class A student threads.
 * You do not need to change anything here, but you can add
 * debug statements to help you during development/debugging.
 */
void* classa_student(void *si)
{
  student_info *s_info = (student_info*)si;

  //
  //printf("***office_sema: %d***\n", sem_val);


  /* enter office  sem_wait*/
  classa_enter();

  printf("Student %d from class A enters the office\n", s_info->student_id);

  assert(students_in_office <= MAX_SEATS && students_in_office >= 0);
  assert(classa_inoffice >= 0 && classa_inoffice <= MAX_SEATS);
  assert(classb_inoffice >= 0 && classb_inoffice <= MAX_SEATS);
  assert(classb_inoffice == 0 ); //while a is executing no b students should be in office



// class, time arrive, time ask questions crit region start

  /* ask questions  --- do not make changes to the 3 lines below*/
  printf("Student %d from class A starts asking questions for %d minutes\n", s_info->student_id, s_info->question_time);
  ask_questions(s_info->question_time);
  printf("Student %d from class A finishes asking questions and prepares to leave\n", s_info->student_id);

  /* leave office sem_post*/
  classa_leave(); // crit region ends

  printf("Student %d from class A leaves the office\n", s_info->student_id);

  assert(students_in_office <= MAX_SEATS && students_in_office >= 0);
  assert(classb_inoffice >= 0 && classb_inoffice <= MAX_SEATS);
  assert(classa_inoffice >= 0 && classa_inoffice <= MAX_SEATS);

  pthread_exit(NULL);
}

/* Main code for class B student threads.
 * You do not need to change anything here, but you can add
 * debug statements to help you during development/debugging.
 */
void* classb_student(void *si)
{
  student_info *s_info = (student_info*)si;


  /* enter office  crit region start */
  classb_enter();

  printf("Student %d from class B enters the office\n", s_info->student_id);

  assert(students_in_office <= MAX_SEATS && students_in_office >= 0);
  assert(classb_inoffice >= 0 && classb_inoffice <= MAX_SEATS);
  assert(classa_inoffice >= 0 && classa_inoffice <= MAX_SEATS);
  assert(classa_inoffice == 0 );

  printf("Student %d from class B starts asking questions for %d minutes\n", s_info->student_id, s_info->question_time);
  ask_questions(s_info->question_time);
  printf("Student %d from class B finishes asking questions and prepares to leave\n", s_info->student_id);

  /* leave office */
  classb_leave(); //crit region end

  printf("Student %d from class B leaves the office\n", s_info->student_id);

  assert(students_in_office <= MAX_SEATS && students_in_office >= 0);
  assert(classb_inoffice >= 0 && classb_inoffice <= MAX_SEATS);
  assert(classa_inoffice >= 0 && classa_inoffice <= MAX_SEATS);

  pthread_exit(NULL);
}

/* Main function sets up simulation and prints report
 * at the end.
 */

int main(int nargs, char **args)
{
  int i;
  int result;
  int student_type;
  int num_students;
  void *status;
  pthread_t professor_tid;
  pthread_t student_tid[MAX_STUDENTS];
  student_info s_info[MAX_STUDENTS];

  if (nargs != 2)
  {
    printf("Usage: officehour <name of inputfile>\n");
    return EINVAL;
  }

  num_students = initialize(s_info, args[1]); //returns total number of students in the file.

  if (num_students > MAX_STUDENTS || num_students <= 0)
  {
    printf("Error:  Bad number of student threads. "
           "Maybe there was a problem with your input file?\n");
    return 1;
  }

  printf("Starting officehour simulation with %d students ...\n", num_students);

  result = pthread_create(&professor_tid, NULL, professorthread, NULL); //professorthread is a multithreading function only one

  if (result)
  {
    printf("officehour: pthread_create failed for professor: %s\n", strerror(result));
    exit(1);
  }


  //starts creating threads based on number of students in the file
  for (i=0; i < num_students; i++)
  {

    s_info[i].student_id = i;   //starts at first student
    sleep(s_info[i].arrival_time); //sleeps at first arrival time

    student_type = random() % 2;

    if (s_info[i].class == CLASSA) // if == 0
    {
      result = pthread_create(&student_tid[i], NULL, classa_student, (void *)&s_info[i]);
    }
    else // student_type == CLASSB
    {
      result = pthread_create(&student_tid[i], NULL, classb_student, (void *)&s_info[i]);
    }

    if (result)
    {
      printf("officehour: thread_fork failed for student %d: %s\n",
            i, strerror(result));
      exit(1);
    }
  }


  /* wait for all student threads to finish TOTAL NUMBER*/
  for (i = 0; i < num_students; i++)
  {
    pthread_join(student_tid[i], &status);
  }

  /* tell the professor to finish. Kills the infinite loop*/
  pthread_cancel(professor_tid);

  printf("Office hour simulation done.\n");

  return 0;
}
