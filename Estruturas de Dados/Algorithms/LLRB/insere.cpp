// http://www.teachsolaisgames.com/articles/balanced_left_leaning.html

QzLLTB_t* LeftLeaningRedBlack::InsertRec(QzLLTB_t* pNode, VoidRef_t ref)
{
    if (NULL == pNode) {
        pNode      = NewNode();
        pNode->Ref = ref;
        return pNode;
    }

#if defined(USE_234_TREE)
    if (IsRed(pNode->pLeft) && IsRed(pNode->pRight))
        ColorFlip(pNode);
#endif

    if (ref.Key == pNode->Ref.Key)
        pNode->Ref = ref;
    else {
        if (ref.Key < pNode->Ref.Key) {
            pNode->pLeft = InsertRec(pNode->pLeft, ref);
        }
        else {
            pNode->pRight = InsertRec(pNode->pRight, ref);
        }
    }

    if (IsRed(pNode->pRight) && !IsRed(pNode->pLeft))
        pNode = RotateLeft(pNode);

    if (IsRed(pNode->pLeft) && IsRed(pNode->pLeft->pLeft))
        pNode = RotateRight(pNode);

#if !defined(USE_234_TREE)
    if (IsRed(pNode->pLeft) && IsRed(pNode->pRight))
        ColorFlip(pNode);
#endif

    return pNode;
}