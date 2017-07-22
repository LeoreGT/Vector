#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "mu_test.h"
#include "vector.h"

typedef struct Person
{
	size_t m_momAge;
	size_t m_dadAge;
	size_t m_personAge;
	int    m_isFemale;
} Person;

/*------------------------------------------------*/

typedef void(*DestroyFunc_t)(void*);

/*------------------------------------------------*/

void KillPerson(Person* _person)
{
	free(_person);
}

/*------------------------------------------------*/

int PrintInfo(Person* _person, size_t _index, int* _factor)
{
	printf("\nIndex:  %lu:\nParents: (Mom: %lu, Dad: %lu)\nMy Age: %lu\nGender: %d", _index, _person->m_momAge, _person->m_dadAge, _person->m_personAge, _person->m_isFemale);
	return 1;
}

/*------------------------------------------------*/
int Multiplym_personAge(Person* _person, size_t _index, int* _factor)
{
	_person->m_personAge *= *_factor;
	return 1;
}

UNIT(create_empty)
	DestroyFunc_t destroyFunc = (DestroyFunc_t) KillPerson;
	Vector* vec = NULL;
	vec = Vector_Create(100, 100);
	ASSERT_THAT(vec != NULL);
	Vector_Destroy(&vec, destroyFunc);
END_UNIT

UNIT(create_zero_size)
	DestroyFunc_t destroyFunc = (DestroyFunc_t) KillPerson;
	Vector* vec = NULL;
	vec = Vector_Create(0, 100);
	ASSERT_THAT(vec != NULL);
	Vector_Destroy(&vec, destroyFunc);
END_UNIT

UNIT(create_non_resizable)
	DestroyFunc_t destroyFunc = (DestroyFunc_t) KillPerson;
	Vector* vec = NULL;
	vec = Vector_Create(100, 0);
	ASSERT_THAT(vec != NULL);
	Vector_Destroy(&vec, destroyFunc);
END_UNIT

UNIT(create_illegal)
	DestroyFunc_t destroyFunc = (DestroyFunc_t) KillPerson;
	Vector* vec = NULL;
	vec = Vector_Create(0, 0);
	ASSERT_THAT(vec == NULL);
	Vector_Destroy(&vec, destroyFunc);
END_UNIT

UNIT(create_and_destroy)
	DestroyFunc_t destroyFunc = (DestroyFunc_t) KillPerson;
	Vector* vec = NULL;
	vec = Vector_Create(100, 100);
	Vector_Destroy(&vec, destroyFunc);
	ASSERT_THAT(vec == NULL);
END_UNIT

UNIT(double_destruction)
	DestroyFunc_t destroyFunc = (DestroyFunc_t) KillPerson;
	Vector* vec = NULL;
	vec = Vector_Create(100, 100);
	Vector_Destroy(&vec, destroyFunc);
	Vector_Destroy(&vec, destroyFunc);
	ASSERT_THAT(vec == NULL);
END_UNIT

UNIT(append_one)
	DestroyFunc_t destroyFunc = (DestroyFunc_t) KillPerson;
	Vector* vec = NULL;
	Person* tempPerson = (Person*) malloc(sizeof(Person));
	vec = Vector_Create(100, 100);
	Vector_Append(vec, tempPerson);
	ASSERT_THAT(Vector_Size(vec) == 1);
	Vector_Destroy(&vec, destroyFunc);
END_UNIT

UNIT(append_few)
	DestroyFunc_t destroyFunc = (DestroyFunc_t) KillPerson;
	Vector* vec = NULL;
	int i;
	Person* tempPerson;
	vec = Vector_Create(100, 100);
	for(i = 0; i < 15; ++i)
	{
		tempPerson = (Person*) malloc(sizeof(Person));
		Vector_Append(vec, tempPerson);
	}
	ASSERT_THAT(Vector_Size(vec) == 15);
	Vector_Destroy(&vec, destroyFunc);
END_UNIT

UNIT(append_many)
	DestroyFunc_t destroyFunc = (DestroyFunc_t) KillPerson;
	Vector* vec = NULL;
	int i;
	Person* tempPerson;
	vec = Vector_Create(100, 100);
	for(i = 0; i < 300; ++i)
	{
		tempPerson = (Person*) malloc(sizeof(Person));
		Vector_Append(vec, tempPerson);
	}
	ASSERT_THAT(Vector_Size(vec) == 300);
	Vector_Destroy(&vec, destroyFunc);
END_UNIT

UNIT(append_many_to_unresizable)
	DestroyFunc_t destroyFunc = (DestroyFunc_t) KillPerson;
	Vector* vec = NULL;
	int i;
	Person* tempPerson;
	vec = Vector_Create(100, 0);
	for(i = 0; i < 300; ++i)
	{
		tempPerson = (Person*) malloc(sizeof(Person));
		if(Vector_Append(vec, tempPerson) != VECTOR_SUCCESS)
		{
			free(tempPerson);
			break;
		}
	}
	ASSERT_THAT(Vector_Size(vec) == 100);
	Vector_Destroy(&vec, destroyFunc);
END_UNIT

UNIT(append_and_remove)
	DestroyFunc_t destroyFunc = (DestroyFunc_t) KillPerson;
	Vector* vec = NULL;
	int i;
	Person* tempPerson;
	vec = Vector_Create(50, 50);
	for(i = 0; i < 100; ++i)
	{
		tempPerson = (Person*) malloc(sizeof(Person));
		Vector_Append(vec, tempPerson);
	}

	for(i = 0; i < 30; ++i)
	{
		Vector_Remove(vec, (void**)&tempPerson);
		free(tempPerson);
	}
	ASSERT_THAT(Vector_Size(vec) == 70);
	Vector_Destroy(&vec, destroyFunc);
END_UNIT

UNIT(set_and_get)
	DestroyFunc_t destroyFunc = (DestroyFunc_t) KillPerson;
	Vector* vec = NULL;
	int i;
	Person* tempPerson;
	Person* pCheck;
	vec = Vector_Create(100, 100);
	for(i = 0; i < 300; ++i)
	{
		tempPerson = (Person*) malloc(sizeof(Person));
		Vector_Append(vec, tempPerson);
	}
	tempPerson = (Person*) malloc(sizeof(Person));
	tempPerson->m_momAge = 100;
	Vector_Set(vec, 5, tempPerson);
	Vector_Get(vec, 5, (void**)&pCheck);
	ASSERT_THAT(pCheck->m_momAge == 100);
	Vector_Destroy(&vec, destroyFunc);
END_UNIT

UNIT(create_and_print)
	VectorElementAction multiplyAction = (VectorElementAction) Multiplym_personAge;
	DestroyFunc_t destroyFunc = (DestroyFunc_t) KillPerson;
	Vector* vec = NULL;
	int i;
	int factor = 2;
	Person* tempPerson;
	vec = Vector_Create(100, 100);
	for(i = 0; i < 5; ++i)
	{
		tempPerson = (Person*) malloc(sizeof(Person));
		tempPerson->m_momAge = rand() % 100;
		tempPerson->m_dadAge = rand() % 100;
		tempPerson->m_personAge = rand() % 100;
		tempPerson->m_isFemale = rand() % 2;
		Vector_Append(vec, tempPerson);
	}
	i =	Vector_ForEach(vec, multiplyAction, &factor);
	ASSERT_THAT(5 == i);
	Vector_Destroy(&vec, destroyFunc);
END_UNIT

TEST_SUITE(Generic Vector Tests)
	TEST(create_empty)
	TEST(create_zero_size)
	TEST(create_non_resizable)
	TEST(create_illegal)
	TEST(create_and_destroy)
	TEST(double_destruction)
	TEST(append_one)
	TEST(append_few)
	TEST(append_many)
	TEST(append_many_to_unresizable)
	TEST(append_and_remove)
	TEST(set_and_get)
	TEST(create_and_print)
END_SUITE
