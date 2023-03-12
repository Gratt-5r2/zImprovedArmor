// Supported with union (c) 2020 Union team

// User API for oCNpc
// Add your methods here

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
bool CanEquipAdditionalArmor( oCItem* item );
void UnequipAdditionalArmorFromWear( int wear );