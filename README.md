# Officehours

<h3>The Problem </h3>

The professor has 2 classes, A and B. He is also holding shared office hours for both classes in his office. Only 3 students in at a time. When office is full and new students arrive they have to wait. Only students from the same class are allowed in at the same time. After seeing 10 students the professor needs to take a break. After 5 consective students from one class the professor will switch to another. 

<h3>Description</h3>

This program uses semaphores and mutex locks to create a multithreaded application that solves the above problem. Three sample file are given to test the program. The format of the file is as follows: The first column is the class, 0 equals class A, while 1 equals class B. The second column is the arrival time of the student and the last column is the amount of time the student spends in the professor's office

<h3>To Run</h3>

To compile: <br>
<code>gcc officehours.c -o office -lpthread</code>

To run: <br>
<code>./office sample.txt</code>
