/**
 *  file    List.h
 *  date    2009/02/23
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui 
 *  brief   리스트에 관련된 함수를 정의한 소스 파일
 */

#include "kList.h"

/**
 *  리스트를 초기화
 */
void kInitializeList( LIST* pstList )
{
    pstList->iItemCount = 0;
    pstList->pvHeader = NULL;
    pstList->pvTail = NULL;
}

/**
 *  리스트에 포함된 아이템의 수를 반환
 */
int kGetListCount( const LIST* pstList )
{
    return pstList->iItemCount;
}

/**
 *  리스트에 데이터를 더함
 */
void kAddListToTail( LIST* pstList, void* pvItem )
{
    LISTLINK* pstLink;
    
    // 다음 데이터의 어드레스를 없음(NULL)으로 설정
    pstLink = ( LISTLINK* ) pvItem;
    pstLink->pvNext = NULL;
    
    // 리스트가 빈 상태이면 Header와 Tail을 추가한 데이터로 설정
    if( pstList->pvHeader == NULL )
    {
        pstList->pvHeader = pvItem;
        pstList->pvTail = pvItem;
        pstList->iItemCount = 1;

        return ;
    }
    
    // 마지막 데이터의 LISTLINK의 위치를 구하여 다음 데이터를 추가한 데이터로 설정
    pstLink = ( LISTLINK* ) pstList->pvTail;
    pstLink->pvNext = pvItem;

    // 리스트의 마지막 데이터를 추가한 데이터로 변경
    pstList->pvTail = pvItem;
    pstList->iItemCount++;
}

/**
 *  리스트의 첫 부분에 데이터를 더함
 */
void kAddListToHeader( LIST* pstList, void* pvItem )
{
    LISTLINK* pstLink;
    
    // 다음 데이터의 어드레스를 Header로 설정
    pstLink = ( LISTLINK* ) pvItem;
    pstLink->pvNext = pstList->pvHeader;    
    
    // 리스트가 빈 상태이면 Header와 Tail을 추가한 데이터로 설정
    if( pstList->pvHeader == NULL )
    {
        pstList->pvHeader = pvItem;
        pstList->pvTail = pvItem;
        pstList->iItemCount = 1;
        
        return ;
    }
    
    // 리스트의 첫 번째 데이터를 추가한 데이터로 변경
    pstList->pvHeader = pvItem;
    pstList->iItemCount++;
}

/**
 *  리스트에서 데이터를 제거한 후, 데이터의 포인터를 반환
 */
void* kRemoveList( LIST* pstList, DWORD qwID )
{
    LISTLINK* pstLink;
    LISTLINK* pstPreviousLink;
    
    pstPreviousLink = ( LISTLINK* ) pstList->pvHeader;
    for( pstLink = pstPreviousLink ; pstLink != NULL ; pstLink = (LISTLINK*)pstLink->pvNext )
    {
        // 일치하는 ID가 있다면 제거
        if( pstLink->qwID == qwID )
        {
            // 만약 데이터가 하나 밖에 없다면 리스트 초기화
            if( ( pstLink == pstList->pvHeader ) && 
                ( pstLink == pstList->pvTail ) )
            {
                pstList->pvHeader = NULL;
                pstList->pvTail = NULL;
            }
            // 만약 리스트의 첫 번째 데이터이면 Header를 두 번째 데이터로 변경
            else if( pstLink == pstList->pvHeader )
            {
                pstList->pvHeader = pstLink->pvNext;
            }
            // 만약 리스트의 마지막 데이터면 Tail을 마지막 이전의 데이터로 변경
            else if( pstLink == pstList->pvTail )
            {
                pstList->pvTail = pstPreviousLink;
            }
            else
            {
                pstPreviousLink->pvNext = pstLink->pvNext;
            }
            
            pstList->iItemCount--;
            return pstLink;
        }
        pstPreviousLink = pstLink;
    }
    return NULL;
}

/**
 *  리스트의 첫 번째 데이터를 제거하여 반환
 */
void* kRemoveListFromHeader( LIST* pstList )
{
    LISTLINK* pstLink;
    
    if( pstList->iItemCount == 0 )
    {
        return NULL;
    }

    // 헤더를 제거하고, 반환
    pstLink = ( LISTLINK* ) pstList->pvHeader;
    return kRemoveList( pstList, pstLink->qwID );
}

/**
 *  리스트의 마지막 데이터를 제거하여 반환
 */
void* kRemoveListFromTail( LIST* pstList )
{
    LISTLINK* pstLink;
    
    if( pstList->iItemCount == 0 )
    {
        return NULL;
    }

    // 테일을 제거하고, 반환
    pstLink = ( LISTLINK* ) pstList->pvTail;
    return kRemoveList( pstList, pstLink->qwID );
}


/**
 *  리스트에서 아이템을 찾음
 */
void* kFindList( const LIST* pstList, DWORD qwID )
{
    LISTLINK* pstLink;
    
    for( pstLink = (LISTLINK*) pstList->pvHeader ; pstLink != NULL ; 
         pstLink = (LISTLINK*)pstLink->pvNext )
    {
        // 일치하는 게 있다면 반환
        if( pstLink->qwID == qwID )
        {
            return pstLink;
        }
    }
    return NULL;    
}

/**
 *  리스트의 헤더를 반환
 */
void* kGetHeaderFromList( const LIST* pstList )
{
    return pstList->pvHeader;
}

/**
 *  리스트의 테일을 반환
 */
void* kGetTailFromList( const LIST* pstList )
{
    return pstList->pvTail;
}

/**
 *  현재 아이템의 다음 아이템을 반환 
 */
void* kGetNextFromList( const LIST* pstList, void* pstCurrent )
{
    LISTLINK* pstLink;
    
    pstLink = ( LISTLINK* ) pstCurrent;

    return pstLink->pvNext;
}
