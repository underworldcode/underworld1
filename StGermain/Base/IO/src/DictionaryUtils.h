#ifndef __StGermain_Base_IO_DictionaryUtils_h__
#define __StGermain_Base_IO_DictionaryUtils_h__

   Dictionary_Entry_Value* _DictionaryUtils_GetRecursive( Dictionary* dict, char* str );
   void _DictionaryUtils_AliasDereferenceStruct( Dictionary* masterDict,  Dictionary_Entry_Value* valuePtr );
   void _DictionaryUtils_AliasDereferenceList( Dictionary* masterDict, Dictionary_Entry_Value* valuePtr);
   void _DictionaryUtils_AliasDereferenceString( Dictionary* masterDict, Dictionary_Entry_Value* valuePtr);
   /* public function for aliases a dictionary */
   void _DictionaryUtils_AliasDereferenceEntryValue( Dictionary* masterDict,  Dictionary_Entry_Value* valuePtr );
   void DictionaryUtils_AliasDereferenceDictionary( Dictionary* masterDict );

#endif

