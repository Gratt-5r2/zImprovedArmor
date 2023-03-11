// Supported with union (c) 2020 Union team

// User API for oCNpc
// Add your methods here

// void EquipItem_Union( oCItem* );
// void UnequipItem_Union( oCItem* );
// void EquipArmor_Union( oCItem* item );
// void UnequipArmorByWear( const int& wear );
// TNpcSlot* CheckInvSlot( const zSTRING& name );
// 
// void InsertSoftSkinItem( oCItem* item );
// void InsertSoftSkin( const zSTRING& visualName, const int& wear );
// void InsertSoftSkin( zCMeshSoftSkin* softSkin, const int& wear );
// 
// void RemoveSoftSkinItem( oCItem* item );
// void RemoveSoftSkin( const int& wear );
// 
// void Unarchive_Union( zCArchiver& );
// bool CanEquipArmorPart( oCItem* item );
// 
// void InitModel_Union();
Array<oCItem*> GetEquippedArmors();
TNpcSlot* CreateAdditionalItemSlot( const zSTRING& name );
void Unarchive_Union( zCArchiver& );
void EquipArmor_Union( oCItem* );
void UnequipItem_Union( oCItem* );
void InitModel_Union();
void EquipAdditionalArmorItem( oCItem* item );
void InsertAdditionalArmorItemToMeshLib( oCItem* item );
void RemoveAdditionalArmorItemFromMeshLib( oCItem* item );
void PutAdditionalArmorItemToSlot( oCItem* item );
void RemoveAdditionalArmorItemFromSlot( oCItem* item );
