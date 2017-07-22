#include <stdlib.h>
#include <stdio.h>
#include "vector.h"

#define HAS_BEEN_ALLOCATED 0xdeadbeef
#define HAS_BEEN_DEALLOCATED 0

#define CHECK(input, inputAgainst, output) if(inputAgainst == input) \
                              { \
                              return output; \
                              }

struct Vector
{
  void** m_items;
  size_t m_size;
  size_t m_initialCapacity;
  size_t m_capacity;
  size_t m_blockSize;
  int    m_vectorMagicNumber;
};

static Vector_Result Vector_Shrink(Vector* _vector);
static Vector_Result Vector_Grow(Vector* _vector);

Vector* Vector_Create(size_t _initialCapacity, size_t _blockSize)
{
  Vector* vector = NULL;

  CHECK(0, _initialCapacity, NULL);
  CHECK(0, _blockSize, NULL);

  vector = (Vector*) malloc(sizeof(Vector));
  CHECK(NULL, vector, NULL);

  vector->m_items = malloc(_initialCapacity * sizeof(void*));
  if(NULL == vector->m_items)
  {
    free(vector);
    return NULL;
  }

  vector->m_size = 0;
  vector->m_initialCapacity = _initialCapacity;
  vector->m_capacity = _initialCapacity;
  vector->m_blockSize = _blockSize;
  vector->m_vectorMagicNumber = HAS_BEEN_ALLOCATED;

  return vector;
}

void Vector_Destroy(Vector** _vector, void (*_elementDestroy)(void* _item))
{
  int indexOfItem, numOfItems;
  if((NULL != _vector) && (NULL != *_vector) && (HAS_BEEN_ALLOCATED == (*_vector)->m_vectorMagicNumber))
  {
    numOfItems = (*_vector)->m_size;
    if(_elementDestroy != NULL)
    {
      for(indexOfItem = 0; indexOfItem < numOfItems; ++indexOfItem)
      {
        _elementDestroy((*_vector)->m_items[indexOfItem]);
      }
    }

    free((*_vector)->m_items);
    (*_vector)->m_vectorMagicNumber = HAS_BEEN_DEALLOCATED;
    free(*_vector);
    *_vector = NULL;
  }
}

Vector_Result Vector_Append(Vector* _vector, void* _item)
{
  if(NULL == _vector || NULL == _item)
  {
    return VECTOR_UNITIALIZED_ERROR;
  }

  /* if reached capacity, reallocate */
  if(_vector->m_size == _vector->m_capacity)
  {
    if(VECTOR_SUCCESS != Vector_Grow(_vector))
    {
      return VECTOR_ALLOCATION_ERROR;
    }
  }

  _vector->m_items[_vector->m_size] = _item;
  ++(_vector->m_size);

  return VECTOR_SUCCESS;
}

Vector_Result Vector_Remove(Vector* _vector, void** _pValue)
{

  CHECK(NULL, _vector, VECTOR_UNITIALIZED_ERROR);
  CHECK(NULL, _pValue, VECTOR_UNITIALIZED_ERROR);

  *_pValue = _vector->m_items[_vector->m_size - 1];
  --(_vector->m_size);

  /* if free space is twice extension block or more, and capacity is larger than initial capacity, then shrink */
  if((_vector->m_capacity - _vector->m_size > _vector->m_blockSize * 2) && _vector->m_capacity > _vector->m_initialCapacity)
  {
    if(VECTOR_SUCCESS != Vector_Shrink(_vector))
    {
      return VECTOR_ALLOCATION_ERROR;
    }
  }

  return VECTOR_SUCCESS;
}

Vector_Result Vector_Get(const Vector* _vector, size_t _index, void** _pValue)
{

  CHECK(NULL, _vector, VECTOR_UNITIALIZED_ERROR);
  CHECK(NULL, _pValue, VECTOR_UNITIALIZED_ERROR);

  CHECK(_index, _vector->m_size, VECTOR_INDEX_OUT_OF_BOUNDS_ERROR);
  CHECK(_index, 0, VECTOR_INDEX_OUT_OF_BOUNDS_ERROR);

  *_pValue = _vector->m_items[_index - 1];
  return VECTOR_SUCCESS;
}

Vector_Result Vector_Set(Vector* _vector, size_t _index, void*  _value)
{
  CHECK(NULL, _vector, VECTOR_UNITIALIZED_ERROR);
  CHECK(NULL, _value, VECTOR_UNITIALIZED_ERROR);

  if(_index > _vector->m_size)
  {
    return VECTOR_INDEX_OUT_OF_BOUNDS_ERROR;
  }

  _vector->m_items[_index - 1] = _value;
  return VECTOR_SUCCESS;
}

size_t Vector_Size(const Vector* _vector)
{
  CHECK(NULL, _vector, 0);
  return _vector->m_size;
}

size_t Vector_Capacity(const Vector* _vector)
{
  CHECK(NULL, _vector, 0);
  return _vector->m_capacity;
}

size_t Vector_ForEach(const Vector* _vector, VectorElementAction _action, void* _context)
{
  int indexOfItem, numOfItems;
  CHECK(NULL, _vector, 0);
  CHECK(NULL, _action, 0);

  numOfItems = _vector->m_size;

  for (indexOfItem = 1; indexOfItem <= numOfItems; ++indexOfItem)
  {
    if (_action(_vector->m_items[indexOfItem - 1], indexOfItem, _context) == 0)
    {
      return indexOfItem;
    }
  }

  return numOfItems;
}

static Vector_Result Vector_Shrink(Vector* _vector)
{
  size_t updatedCapacity;
  void* temp;
  temp = _vector->m_items;

  /* new capacity is the larger one: current - one extension block OR initial capacity */
  updatedCapacity = (_vector->m_capacity - _vector->m_blockSize > _vector->m_initialCapacity ? _vector->m_capacity - _vector->m_blockSize : _vector->m_initialCapacity);
  _vector->m_items = realloc(_vector->m_items, updatedCapacity * sizeof(void*));

  CHECK(NULL, _vector->m_items, VECTOR_ALLOCATION_ERROR);

  _vector->m_items = temp;
  _vector->m_capacity = updatedCapacity;

  return VECTOR_SUCCESS;
}

static Vector_Result Vector_Grow(Vector* _vector)
{
  void* temp;
  CHECK(0, _vector->m_blockSize, VECTOR_ALLOCATION_ERROR);

  temp = _vector->m_items;
  _vector->m_items = realloc(_vector->m_items, (_vector->m_capacity + _vector->m_blockSize) * sizeof(void*));

  if(NULL == _vector->m_items)
  {
    _vector->m_items = temp;
    return VECTOR_ALLOCATION_ERROR;
  }
  _vector->m_capacity += _vector->m_blockSize;
  return VECTOR_SUCCESS;
}
