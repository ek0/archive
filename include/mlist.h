#ifndef _MLIST_H_
#define _MLIST_H_

#include <Windows.h>

inline void InitializeListHead(LIST_ENTRY* list_head)
{
    list_head->Flink = list_head;
    list_head->Blink = list_head;
}

inline int IsListEmpty(LIST_ENTRY* list_head)
{
    return list_head->Flink == list_head;
}

inline void InsertHeadList(LIST_ENTRY* list_head, LIST_ENTRY* entry)
{
    LIST_ENTRY* next;

    next = list_head->Flink;
    entry->Flink = next;
    entry->Blink = list_head;
    next->Blink = entry;
    list_head->Flink = entry;
}

inline void InsertTailList(LIST_ENTRY* list_head, LIST_ENTRY* entry)
{
    LIST_ENTRY* prev;

    prev = list_head->Blink;
    entry->Flink = list_head;
    entry->Blink = prev;
    prev->Flink = entry;
    list_head->Blink = entry;
}

inline void RemoveEntryList(LIST_ENTRY* entry)
{
    LIST_ENTRY* prev;
    LIST_ENTRY* next;

    prev = entry->Blink;
    next = entry->Flink;
    prev->Flink = next;
    next->Blink = prev;
}

inline LIST_ENTRY* RemoveHeadList(LIST_ENTRY* list_head)
{
    LIST_ENTRY* entry;

    entry = list_head->Flink;
    RemoveEntryList(entry);
    return entry;
}

inline LIST_ENTRY* RemoveTailList(LIST_ENTRY* list_head)
{
    LIST_ENTRY* entry;

    entry = list_head->Blink;
    RemoveEntryList(entry);
    return entry;
}

#endif // _MLIST_H_