#ifndef _MITK_STL_VECTOR_CONTAINER_TXX_
#define _MITK_STL_VECTOR_CONTAINER_TXX_

#include "mitkSTLVectorContainer.h"

template<typename T>
unsigned int mitk::STLVectorContainer<T>::GetSize()
{
    return this->Get().size();    
}

template<typename T>
void mitk::STLVectorContainer<T>::Clear()
{
    return this->Get().clear();    
    this->Modified();
}

/*
 * Resizes the underlying stl vector to the given size.
 */
template<typename T>
void mitk::STLVectorContainer<T>::Resize( unsigned int size )
{
    this->Get().resize( size );    
    this->Modified();
}

/*
 * sets the n'th element of the underluing stl vector. 
 * if the index is out of range, an assertion is raised
 */
template<typename T>
void mitk::STLVectorContainer<T>::SetElement( const unsigned int& index, const typename mitk::STLVectorContainer<T>::ValueType& element )
{
    assert( index < this->Get().size() );
    this->Get()[index] = element;
    this->Modified();
}

/*
 * returns the n'th element of the undelying stl vector
 * If the index is out of range, an assertion is raised!
 */
template<typename T>
typename mitk::STLVectorContainer<T>::ValueType& mitk::STLVectorContainer<T>::GetElement( const unsigned int& index )
{
    assert( index < this->Get().size() );
    return this->Get()[index];
    
}
        

#endif
