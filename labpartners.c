    /***********************************************************************
     * Name: Peter Murphy                                                  *
     * Assignment name ( Homework 3: Assigning Lab Partners)               *
     *                                                                     *
     * Assignment for Dec 09 2022                                          *
     ***********************************************************************/

    /* *********************************************************************
     * Academic honesty certification:                                     *
     *   Written/online sources used:                                      *
     *   -namelist.c from linked list lab from                             *
     * https://johnsonba.cs.grinnell.edu/CSC161/2022F/lists/labs/linked... * 
     *   - C Programming by K.N.King chapters 22.1, 22.2 22.3, 22.4, 17.3  *
     *   -  https://c-for-dummies.com/blog/?p=4236 for gettimeofday()      *
     *   Help obtained                                                     *
     *     [Professor Barbara Johnson]                                     *
     *                                                                     *
     *   My/our signature(s) below confirms that the above list of sources *
     *   is complete AND that I/we have not talked to anyone else          *
     *   (e.g., CSC 161 students) about the solution to this problem       *
     *                                                                     *
     *   Signature: Peter Murphy                                           *
     ***********************************************************************/

//This program will take an input file and generate different pairs of students in the output file
//The user can determine whether to use the random or determinstic algorithm
//both algorithms successfully work with 2 to 150 students as inputs
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/time.h>
#define MAXSTR 100

typedef struct node Student;

//define firstname, lastname, number(which will be between 1 and listlength), previousPartnerArray(an array of numbers that were recently paired with our student)
//partner1Partner is a pointer to the Student the current Student will be paired with, partner2Pointer will usually be null, next is the next student in the linked list
struct node {
    char * firstName;
    char * lastName;
    int number;
    int * previousPartnerArray;
    Student *partner1Pointer;
    Student *partner2Pointer;
    Student * next;
};

//Pre: An int greater (exclusive) than zero
//Post: int 0-9
//Pseudorand gives a pseudo random digit by looping through digits (post decimal) of pi, then ln(10), sqrt(2), e, pi/4
int pseudorand(int digit)
{   
    if (digit < 1)
    {
        fprintf(stderr, "Error, input for pseudorand function was 0 or less"); //violates pre
        return -1;
    }
    if (digit > 90)
    {
        digit = ((digit - 1) % 90) + 1; //always gives output range 1-90
    }
    double outputdouble;
    if(digit <= 15)
    {
        outputdouble = M_PI;//all thesese constants are defined in math.h
    }
    else if(digit <= 30)
    {
        outputdouble = M_LN10;
    }
    else if(digit <= 45)
    {
        outputdouble = M_SQRT2;
    }
    else if(digit <= 60)
    {
        outputdouble = M_E;
    }
    else if(digit <= 75)
    {
        outputdouble = M_LOG2E;
    }
    else// digits <= 90
    {
        outputdouble = M_PI_4;
    }
    digit = ((digit - 1) % 15) + 1; //which digit of pi(or any other irrational) we have to loop through
    while (digit > 0)
    {
        outputdouble = fmod((outputdouble * 10), 10); //multiplying by 10s as these decimals will be small
        digit--;
    }
    int outputint = (int) outputdouble; //casting to int
    return outputint;
}

//Pre: seed: int greater than zero (exclusive), digits: between 1 and 9
//Post: Integer zero or greater
//Gives a random number of at most digits (because the first digit could be zero) using pseudorand
int pseudorandomdigits(int seed, int digits)
{
    if (digits > 9)
    {
        printf("Too many digits for random digit function\n");
        return -1;
    }
    int output = 0;
    int inputforpseudorand;
    for (int i = 0; i < digits; i++)
    {
        inputforpseudorand = seed + (1 + i * 593); //the '593' has to be some prime number (we lost randomness when it was 10)
        output += pow(10, i) * pseudorand(inputforpseudorand); //adding more digits
    }
    return output;
}

//Pre: studentlist is the head of a linked list, extracted stu is a pointer inside the linked list
//Post: the extracted (new) Student will be put at the front of the linked list
//This function accounts for studentlist being empty
void moveToFront (Student ** studentlist, Student ** extractedStu) 
{
    Student *prev = *studentlist;
    Student *current = (*studentlist)->next;
    Student *following = (*studentlist)->next->next;

    if (prev == *extractedStu) //the first thing in the student list is the extractedStu and we don't need to do anything
    {
        return;
    }
    while((current != *extractedStu) && (following->next != NULL))//while we haven't hit extractedStu keep looping
    { 
        prev = prev->next;
        current = current->next;
        following = following->next;
    }
    if (current == *extractedStu)
    {
        Student *temp = *studentlist;
        *studentlist = current; //move current to front
        current->next = temp;
        prev->next = following;//reconnecting the links between prev and next
    }
    else if (following  == *extractedStu) //if false we check the last node (because current will stop at the second to last node in the while loop)
    {
        current->next = NULL; //this is necessary or the linked list is a never ending circle
        following->next = *studentlist;//we don't need to connect paths because the second to last node become the last
        *studentlist = following;
    }
    else
    {
        fprintf(stderr,"The inputs to movetoFront didn't meet the preconditions\n");
    }
}

//Pre: myIntArray is an array of ints, myInt is any int, listLength is the the length of myIntArray
//Post: A boolean
//Returns true if the myInt is in the array otherwise returns false
bool intIsInMyArray(int myIntArray[], int myInt, int listLength)
{
    for(int i = 0; i < listLength; i++) //loop through entire array
    {
        if (myInt == myIntArray[i])
        {
            return true;
        }
    }
    return false;
}

//Pre: studentlist is the head of the student linked list, listlength is an int of the length of that linked list
// randomNumberSeed is an int greater than zero (hopefully large though), offset is an int
//Post: A new randomNumberSeed (an int)
//shuffle will shuffle the linked list using pseudorandomdigits, this shuffling algorithm is deterministic as its behavior will be the same with the same seed
int shuffle(Student **studentlist, int listLength, int randomNumberSeed, int offset)//we make an assumption that max Listlength is 99999
{
    int indexCounter;
    int randomIndex = pseudorandomdigits(randomNumberSeed, 7) % listLength;
    Student *temp;
    for(int i = 0; i < listLength; i++)
    {
        randomNumberSeed = (pseudorandomdigits(randomNumberSeed + (i * 5) + (randomIndex * 7) + ((*studentlist)->number * 11) + (offset * 13), 7));
        randomIndex = randomNumberSeed % listLength; //index has to be less than listlength
        indexCounter = 0;
        temp = *studentlist;
        while(indexCounter < randomIndex) //temp should never be null because random index always less than list length
        {
            if(temp == NULL)
            {
                fprintf(stderr,"randomIndex out of bounds\n");
                break;
            }
            temp = temp->next; //select the randomIndex element of the list and send it to the front (this is a way to shuffle) 
            indexCounter++;
        }
        moveToFront(studentlist, &temp); 
    }
    return randomNumberSeed;
}

//Pre: currentStudentptr is a pointer to a Student node, studentList is the head of the student linked list
//Post: void
//Unlike moveStudentToStart which was used for shuffle, this is used for adding new Students during the initialization of the linked list
//currentStudentptr is not in studentList as a pre condition
void moveStudentToEnd(Student ** currentStudentptr, Student ** studentList)
{
	(*currentStudentptr)->next = NULL;
	Student *temp = *studentList;
	if (temp == NULL) //no elements in target list
	{
		*studentList = *currentStudentptr;//the list is the one student if the list is empty
	}
	else //there are elements in target list
	{
		while(temp->next != NULL) //while we haven't reached the last student in the list
		{
			temp = temp->next; //move up one student
		}
		temp->next = *currentStudentptr; //when we have reached the last student, the next one is the 1st param student
	}
}

//Pre: ptr_to_studentlist is a linked list of students, studentNumber is a number given the the student (this is helpful for adding partners) (not the length of the list)
//firstnameInput and lastnameinput are string that will be put into the fields of the new Student
//Post: A bool (true) if we were successfull (dependant on if we ran out of memory)
//Filling out the fields for the student we are adding to the linked list
bool initializeStudent(Student ** ptr_to_studentList, int studentNumber, char * firstNameInput, char * lastNameInput)
{
    bool success = false;
	Student * newStu = malloc(sizeof(Student));
	if (newStu == NULL) {
		return success; 
	}
    newStu->firstName = malloc(strlen(firstNameInput) + 1); //we have to allocate mem to string fields
    if (!(newStu->firstName))
    {
        return success;
    }
    strcpy(newStu->firstName, firstNameInput);
    newStu->lastName = malloc(strlen(lastNameInput) + 1);
    if (!(newStu->lastName))
    {
        return success;
    }
    strcpy(newStu->lastName, lastNameInput);
    success = true; //we did not run ouf memory
    newStu->number = studentNumber; //filling out fields
    newStu->previousPartnerArray = NULL;
    newStu->partner1Pointer = NULL;
    newStu->partner2Pointer = NULL;
    newStu->next = NULL;
    moveStudentToEnd(&newStu, ptr_to_studentList);
    return success;
}

//Pre: fpr is our read file pointer, ptr_to_emptuStudentList is where our linked list will be located (should be null as a precondition)
//Post: A bool (should be true if we didn't run out of memory)
//we initialize the entire Student linked list by reading our input file
bool initializeStudentLinkedList(FILE *fpr, Student **ptr_to_emptyStudentList)
{
    int nameLoops = 0;
    char firstnametemp[MAXSTR];
    char lastnametemp[MAXSTR];
    int firstnameIndex = 0;
    int lastnameIndex = 0;
    char ch;
    while((ch = getc(fpr)) != EOF)
    {
        firstnameIndex = 0;
        lastnameIndex = 0;
        firstnametemp[firstnameIndex] = '\0';
        lastnametemp[lastnameIndex] = '\0';
        ungetc(ch, fpr);
        while((ch = getc(fpr)) != ',') //our input file has lastname first
        {
            lastnametemp[lastnameIndex] = ch;
            lastnameIndex++;
        }
        lastnametemp[lastnameIndex] = '\0'; //end string with null character to not get garbage in our string
        while((ch = getc(fpr)) != '\n')//first name is up until newline
        {
            firstnametemp[firstnameIndex] = ch;
            firstnameIndex++;
        }
        firstnametemp[firstnameIndex] = '\0';
        nameLoops++;//used for giving the student their number
        if (!(initializeStudent(ptr_to_emptyStudentList, nameLoops, firstnametemp, lastnametemp)))
        {
            fprintf(stderr, "Couldn't created linked Lists of Students");
            return false;
        }
    }
    return true;
}

//Pre: ptr_to_studentlist is the linked list of students, assignDayCount is which set number we are on, previousArrLength is the length of the array of previous partner numbers
//randAlgorithm is a bool on whether to use the random or deterministic partner assignment algorithm
//Post: Void
//Clears partners and adds their numbers to the previous partner number list
void partnersArePrevious(Student *ptr_to_studentlist, int assignDayCount, int previousArrLength, bool randAlgorithm)
{
    Student *temp = ptr_to_studentlist;
    while (temp != NULL)
    {   
        if (!randAlgorithm)
        {
            temp->previousPartnerArray[((assignDayCount - 1) % previousArrLength)] = temp->partner1Pointer->number; //add the number to the array
            if(temp->partner2Pointer != NULL)
            {
                temp->previousPartnerArray[(((assignDayCount - 1) + (previousArrLength - 1)) % previousArrLength)] = temp->partner2Pointer->number; //we have to the number of partner2 to the back to not override it in the next loop
                temp->partner2Pointer = NULL;
            }
            temp->partner1Pointer = NULL;
        }
        else //random algorithm
        {
            temp->partner1Pointer = NULL; //random algorithm doesn't need a previous partner array
            temp->partner2Pointer = NULL;
        }
        temp = temp->next;
    }
}


//Pre: ptr_to_studentlist is the beginning of the student linked list, listlength is its length, setnumber is what set number to print, fpw is the pointer to the output file
//Post: void
//Prints already assigned partners to the target output file
void printLabParterns(Student *ptr_to_studentlist, int listlength, int setnumber, FILE *fpw)
{
    int *arrayofprinted = calloc(listlength, sizeof(int)); //keeps track of what students we already printed
    if (!arrayofprinted)
    {
        fprintf(stderr,"Calloc memory error");
        return;
    }
    Student *temp = ptr_to_studentlist;
    int indexCounter = 0;
    fprintf(fpw, "-------- Set #%d --------\n", setnumber);
    while (temp != NULL)
    {
        if (!intIsInMyArray(arrayofprinted, temp->number, listlength))
        {
            indexCounter = 0;
            if (temp->partner2Pointer == NULL)// group of 2
            {
                fprintf(fpw,"%s %s and %s %s\n", temp->firstName, temp->lastName, temp->partner1Pointer->firstName, temp->partner1Pointer->lastName);
                while(arrayofprinted[indexCounter] != 0)
                {
                    indexCounter++;
                }
                arrayofprinted[indexCounter] = temp->number; //put the numbers of the printed students in the arrayofprinted integer array
                arrayofprinted[indexCounter + 1] = temp->partner1Pointer->number;
            }
            else //group of 3
            {
                fprintf(fpw, "%s %s and %s %s and %s %s\n", temp->firstName, temp->lastName, temp->partner1Pointer->firstName, temp->partner1Pointer->lastName, temp->partner2Pointer->firstName, temp->partner2Pointer->lastName);
                while(arrayofprinted[indexCounter] != 0)
                {
                    indexCounter++;
                }
                arrayofprinted[indexCounter] = temp->number;
                arrayofprinted[indexCounter + 1] = temp->partner1Pointer->number;
                arrayofprinted[indexCounter + 2] = temp->partner2Pointer->number;
            }
        }
        temp = temp->next;
    }
}

//Pre: Pointer to the head of the student linked list
//Post: Void
//makes sure all partners are set to nothing
void clearPartnerPointers(Student *ptr_to_studentlist)
{
    Student *temp = ptr_to_studentlist;
    while (temp != NULL) //loop through all students
    {
        temp->partner1Pointer = NULL;
        temp->partner2Pointer = NULL;
        temp = temp->next;
    }
}

//Pre: firstudentlist is the head of stundent linked list, previousArrLength is the length of the previous lab partner number array
//Post: Bool (true if we didn't run out of memory)
bool initializePreviousArrays(Student *firststudentlist, int previousArrLength)
{
    Student * temp = firststudentlist;
    while (temp != NULL)
    {
        temp->previousPartnerArray = calloc(previousArrLength, sizeof(int)); //Set the array to a bunch of zeros
        if (temp->previousPartnerArray == NULL) 
        {
            fprintf(stderr, "Calloc memory error\n");
		    return false; //ran out of memory
	    }
        temp = temp->next;
    }
    return true;
}

//Pre: currentStu is a student node in a linked list, so is stutobechecked (they should not be the same student), previousArrLength is an int of the length of previous partner int arrays
//Post: a bool(true if we didn't run out of memory)
//check if the Student we want to check's number is in current students previous partner number list (if yes the two students can't be partners)
bool checkIfPrevious(Student *currentStu, Student *stuToBeChecked, int previousArrLength)
{
    for (int i = 0; i < previousArrLength; i++)
    {
        if(currentStu->number == stuToBeChecked->previousPartnerArray[i])
        {
            return true;
        }
    }
    return false;
}

//Pre: the first student in our student linked lsit
//Post: An integer
//Returns an integer of the number of students in our linked list who don't have partners yet
int unmatchedStudents(Student *studentlist)
{
    Student * temp = studentlist;
    int unMatchedCount = 0;
    while (temp != NULL)
    {
        if (temp->partner1Pointer == NULL)//if partner1 is null we don't have partners (partner2 will also be null if this is true)
        {
            unMatchedCount++; //increase the count by 1
        }
        temp = temp->next;
    }
    return unMatchedCount;
}

//Pre: unAvailableNumberlist is an array of integeres corresponding to the numbers of students who have already been matched with partners,
//available is the array we will be changing that will only have available numbers of students (with no zeros since we will be mallocing), length is the length of the student linked list
//Post: Void
//We create an array of available student numbers so that we can randomly select one to parter up with another person from the same list
void createArrayOfAvailableNumbers (int * unAvailableNumberList, int **availableNumberList, int length)
{
    int availableNumberIndex = 0;
    int lengthAvailableList = 0;
    for(int numberWeAreChecking = 1; numberWeAreChecking <= length; numberWeAreChecking++)
    {
        if (!intIsInMyArray(unAvailableNumberList, numberWeAreChecking, length))
        {
            lengthAvailableList++;//we will be allocating more memory if there more available students
        }
    }
    *availableNumberList = calloc(lengthAvailableList, sizeof(int));
    if (*availableNumberList == NULL)
    {
        fprintf(stderr,"Calloc memory Error\n");
        return;
    }
    for(int numberWeAreChecking = 1; numberWeAreChecking <= length; numberWeAreChecking++)//loop through all students
    {
        if (!intIsInMyArray(unAvailableNumberList, numberWeAreChecking, length))//if the student is available add their number to the list
        {
            (*availableNumberList)[availableNumberIndex] = numberWeAreChecking;
            availableNumberIndex++;//increase the index to the available number lsit
        }
    }
}

//Pre: double pointer to the first student in our student list, length is the length of this list
//Post: Void
//This is our random partner assignment algorithm
void assignLabPartnersRandom(Student **ptr_to_studentlist, int length)
{
    struct timeval tv;
    struct timezone tz; //used for seeding pseudorandomdigits (gettimeofday)
    if(length < 2)
    {
        fprintf(stderr,"Invalid student list length\n"); 
        return;
    }
    int * alreadySelectedArray;
    alreadySelectedArray = calloc(length, sizeof(int));
    if (!alreadySelectedArray)//if null pinter
    {
        fprintf(stderr,"Calloc memory error\n");
        return;
    }
    int * availableNumberArray;
    int availableNumberArrayLength = length;
    int randInt1 = 0;
    int randInt2 = 0; //randomIndexes we will choose our random partners with
    int alreadySelectedCounter = 0;
    Student *temp1;
    Student *temp2;
    while (unmatchedStudents(*ptr_to_studentlist) > 3)//keep assigning parters of 2 until there are 3 or less unmatched students left
    {
        createArrayOfAvailableNumbers(alreadySelectedArray, &availableNumberArray ,length);
        gettimeofday(&tv,&tz);
        randInt1 = availableNumberArray[pseudorandomdigits(((int)tv.tv_usec), 7) % availableNumberArrayLength];//select random index that will for sure be available
        alreadySelectedArray[alreadySelectedCounter] = randInt1; //add this element to the already selected array so we don't select it again
        alreadySelectedCounter++;
        availableNumberArrayLength--;//there are now less available Students
        createArrayOfAvailableNumbers(alreadySelectedArray, &availableNumberArray ,length);
        gettimeofday(&tv,&tz);
        randInt2 = availableNumberArray[pseudorandomdigits(((int)tv.tv_usec), 7) % availableNumberArrayLength];
        alreadySelectedArray[alreadySelectedCounter] = randInt2;
        alreadySelectedCounter++;
        availableNumberArrayLength--;
        temp1 = *ptr_to_studentlist;
        while(temp1->number != randInt1)
        {
            temp1 = temp1->next; //loop to get to the correct student based on the randomIndex
        }
        temp2 = *ptr_to_studentlist;
        while(temp2->number != randInt2)
        {
            temp2 = temp2->next;
        }
        temp1->partner1Pointer = temp2;
        temp2->partner1Pointer = temp1;
    }//there are 3 or two students remaining
    if (unmatchedStudents(*ptr_to_studentlist) == 3) //if we have 3 left
    {
        Student *firstStuGroup3 = *ptr_to_studentlist;
        Student *secondStuGroup3 = *ptr_to_studentlist;
        Student *thirdStuGroup3 = *ptr_to_studentlist; //we will now assign the last 3 students together as partners
        while(firstStuGroup3->partner1Pointer != NULL)
        {
            firstStuGroup3 = firstStuGroup3->next;
        }
        secondStuGroup3 = firstStuGroup3 ->next;
        while(secondStuGroup3->partner1Pointer != NULL)
        {
            secondStuGroup3 = secondStuGroup3->next;
        }
        thirdStuGroup3 = secondStuGroup3 ->next;
        while(thirdStuGroup3->partner1Pointer != NULL)
        {
            thirdStuGroup3 = thirdStuGroup3->next;
        }//we have now found the 3 students that are remaining
        firstStuGroup3->partner1Pointer = secondStuGroup3;
        firstStuGroup3->partner2Pointer = thirdStuGroup3;
        secondStuGroup3->partner1Pointer = firstStuGroup3;
        secondStuGroup3->partner2Pointer = thirdStuGroup3;
        thirdStuGroup3->partner1Pointer = firstStuGroup3;
        thirdStuGroup3->partner2Pointer = secondStuGroup3;//partner assigment
    } 
    else if (unmatchedStudents(*ptr_to_studentlist) == 2)
    {
        Student *firstStuGroup2 = *ptr_to_studentlist;
        Student *secondStuGroup2 = *ptr_to_studentlist;
        while(firstStuGroup2->partner1Pointer != NULL)
        {
            firstStuGroup2 = firstStuGroup2->next;
        }
        secondStuGroup2 = firstStuGroup2 ->next;
        while(secondStuGroup2->partner1Pointer != NULL)
        {
            secondStuGroup2 = secondStuGroup2->next;
        } //we have now found the 2 students that are remaining
        firstStuGroup2->partner1Pointer = secondStuGroup2;
        secondStuGroup2->partner1Pointer = firstStuGroup2;
    }
}

//Pre: double pointer to the beginning our of studentlist, length is the length of this list, previousarrlength is the length of our previous student number array
//Post: bool (did we not run out of memory?)
//this is our deterministic partner assignment algorithm
bool assignLabPartnersDeterm (Student **ptr_to_studentlist, int previousArrLength, int length)
{
    Student *current = *ptr_to_studentlist;
    Student *after = (*ptr_to_studentlist)->next;
    if (length < 2)
    {
        fprintf(stderr,"Invalid student list length\n");
    }
    else if (length == 2)
    {
        current->partner1Pointer = after;
        after->partner1Pointer = current;
    }
    else if (length == 3)
    {
        current->partner1Pointer = after;
        current->partner2Pointer = after->next;
        after->partner1Pointer = current;
        after->partner2Pointer = after->next;
        after->next->partner1Pointer = current;
        after->next->partner2Pointer = after;
    }//if there aren't that many students we just assign them together as partners because we have no other options
    else if (length % 2 == 0) //if there will be an even student count (no groups of 3)
    {
        while(current!=NULL)//loop through the linked list
        {
            if (current->partner1Pointer == NULL) //if the student we are looking at doesn't have a partner
            {
                if (current->next != NULL)
                {
                    after = current->next; //if we can create an 'after' student pointer then make it
                }
                while((after != NULL) && ((checkIfPrevious(current, after, previousArrLength)) || (after->partner1Pointer != NULL)))//loop if the next pointer isn't null and we shouldn't select the current Student as a partner (for the 2 reasons)
                {
                    after = after->next;
                }
                if ((after != NULL))//we have found a pair: current, after
                {
                    current->partner1Pointer = after;
                    after->partner1Pointer = current;
                    current = current->next;
                }
                else //we couldn't find a pair for current (everybody has been with currentpointer recently)
                {
                    return false; //we will have to shuffle the list because in our current order a Student only ends up with a selection of students that have already been with them
                }
            }
            else //current already has a partner
            {
                current = current->next;
            }
        }
    }
    else //odd number of students
    {
        while ((current != NULL) && (unmatchedStudents(*ptr_to_studentlist) > 3)) //loop while there are only 3 students remaining
        {//do the same steps as with the even number of students
            if (current->partner1Pointer == NULL) //if we don't have a partner
            {
                if (current->next != NULL)
                {
                    after = current->next;
                }
                while((after != NULL) && ((checkIfPrevious(current, after, previousArrLength)) || (after->partner1Pointer != NULL)))
                {
                    after = after->next;
                }
                if ((after != NULL))//we have found a pair: current, after
                {
                    current->partner1Pointer = after;
                    after->partner1Pointer = current;
                    current = current->next;
                }
                else //we couldn't find a pair for current (everybody has been with currentpointer recently)
                {
                    return false; //we couldn't achieve a partner for everybody and will need to shuffle the list to get a more favorable order
                }
            }
            else //current already has a partner
            {
                current = current->next;
            }
        }
        if (unmatchedStudents(*ptr_to_studentlist) == 3)//if we have 3 unmatched students remaining
        {
            Student *firstStuGroup3 = *ptr_to_studentlist;
            Student *secondStuGroup3 = *ptr_to_studentlist;
            Student *thirdStuGroup3 = *ptr_to_studentlist;//we will now find these last 3 students
            while(firstStuGroup3->partner1Pointer != NULL)
            {
                firstStuGroup3 = firstStuGroup3->next;
            }
            secondStuGroup3 = firstStuGroup3 ->next;
            while(secondStuGroup3->partner1Pointer != NULL)
            {
                secondStuGroup3 = secondStuGroup3->next;
            }
            thirdStuGroup3 = secondStuGroup3 ->next;
            while(thirdStuGroup3->partner1Pointer != NULL)
            {
                thirdStuGroup3 = thirdStuGroup3->next;
            }//we have now found the 3 students that are remaining
            if ((!(checkIfPrevious(firstStuGroup3, secondStuGroup3, previousArrLength))) && (!(checkIfPrevious(secondStuGroup3, thirdStuGroup3, previousArrLength))) && (!(checkIfPrevious(thirdStuGroup3, firstStuGroup3, previousArrLength))))
            { //we are checking if the haven't been with each other recently
                firstStuGroup3->partner1Pointer = secondStuGroup3;//assign them together
                firstStuGroup3->partner2Pointer = thirdStuGroup3;
                secondStuGroup3->partner1Pointer = firstStuGroup3;
                secondStuGroup3->partner2Pointer = thirdStuGroup3;
                thirdStuGroup3->partner1Pointer = firstStuGroup3;
                thirdStuGroup3->partner2Pointer = secondStuGroup3;
            }
            else
            {
                return false; //they have been with each other recently and we will need to start over with a shuffled list
            }
        }
    }
    return true;
}

int main(int argc, char *argv[])
{
    FILE * fpr = NULL; //create file pointer
    FILE * fpw = NULL;

    bool randomProgram = ((!(strcmp(argv[1],"-r")) && (argc == 4)) || (!(strcmp(argv[1],"-R")) && (argc == 4))); //determine if we will run the random algorithm based on flags

    if ((argc != 3) && !randomProgram) //the user gave invalid inputs to main
    {
        fprintf(stderr, "Incorrect number of files\n");
        exit(EXIT_FAILURE);
    }

    char* input_file_name = malloc(sizeof(char)*50);
    char* output_file_name = malloc(sizeof(char)*50);
    if (randomProgram)
    {
        strcpy(input_file_name,argv[2]);
        strcpy(output_file_name,argv[3]);
    }
    else
    {
        strcpy(input_file_name,argv[1]);
        strcpy(output_file_name,argv[2]);
    }//we have now set out input and output files

    if ((fpr = fopen(input_file_name, "r")) == NULL)
    {
        fprintf(stderr, "%s can't be opened\n", input_file_name);
        exit(EXIT_FAILURE);
    }
    if ((fpw = fopen(output_file_name, "w")) == NULL)
    {
        fprintf(stderr, "%s can't be opened\n", output_file_name);
        exit(EXIT_FAILURE);
    }

    char ch;
    int studentStrIndex = 0;
    char studentNumbStr[MAXSTR];
    while ((ch = getc(fpr)) != '\n')
    {
        studentNumbStr[studentStrIndex] = ch;
        studentStrIndex++;
    }
    studentNumbStr[studentStrIndex] = '\0';
    int studentNumb = atoi(studentNumbStr);//convert our studentnumber from a string to int so we can use it as our "length" variable in numerous functions

    int setNumbStrIndex = 0;
    char setNumbStr[MAXSTR];
    while ((ch = getc(fpr)) != '\n')
    {
        setNumbStr[setNumbStrIndex] = ch;
        setNumbStrIndex++;
    }
    setNumbStr[setNumbStrIndex] = '\0';
    int setNumb = atoi(setNumbStr); //this will be our loop number for our set print function

    Student * myStudentList;
    myStudentList = NULL;
    initializeStudentLinkedList(fpr, &myStudentList); //create our initial linked list
    fclose(fpr); //we have initialized our linked lists with the appropriate names and don't need to read from the input file anymore

    int randomInt = 111111;//this will become randomized in our deterministic algorithm
    int prevArrayLength = ((int) (studentNumb/2)); //we don't want our previous array to be too long or our pseudorandomness will be somewhat lost
    if (studentNumb == 5) //special scenario when length = 5, by default prevarraylength = 2 for a length of 5 and we get an infinite loop during assignlabparterns
    {
        prevArrayLength = 1;
    }
    int setCounter = 0;
    clearPartnerPointers(myStudentList);
    if (!randomProgram)
    {
        initializePreviousArrays(myStudentList, prevArrayLength);
        int shufflecounter = 0;
        for (int i = 0; i < setNumb; i++)
        {
            while(!assignLabPartnersDeterm(&myStudentList, prevArrayLength, studentNumb)) //we couldn't assign partners
            {
                randomInt = shuffle(&myStudentList, studentNumb, randomInt, shufflecounter);//changing the order of students
                clearPartnerPointers(myStudentList);
                shufflecounter++; //this is used as an offset to ensure randomness
            }
            printLabParterns(myStudentList, studentNumb, i + 1, fpw);
            partnersArePrevious(myStudentList, setCounter + 1, prevArrayLength, randomProgram);
            setCounter++;
        }
    }
    else
    {
        for (int i = 0; i < setNumb; i++)
        {
            assignLabPartnersRandom(&myStudentList, studentNumb);
            printLabParterns(myStudentList, studentNumb, i + 1, fpw);
            partnersArePrevious(myStudentList, setCounter + 1, prevArrayLength, randomProgram);
            setCounter++;
        }
    }
    fclose(fpw);

    char algorithm[14];
    if (randomProgram)
    {
        strcpy(algorithm, "random");
    }
    else
    {
        strcpy(algorithm, "deterministic");
    }
    printf("Successfully printed the %d sets of pairs to %s with the %s algorithm\n", setNumb, output_file_name, algorithm);
}
    
