// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	TNpcSlot* oCNpc::CheckInvSlot( const zSTRING& name ) {
		TNpcSlot* slot = GetInvSlot( name );
		if( slot )
			return slot;

		cmd << "New slot: " << name << endl;
		CreateInvSlot( name );
		return GetInvSlot( name );
	}



	void oCNpc::InsertSoftSkinItem( oCItem* item ) {
		if( !item )
			return;

		int wear = GetFirstWearSlot( item->wear );
		zSTRING slotName = SlotPrefix + wear;
		TNpcSlot* slot = CheckInvSlot( slotName );
		slot->SetVob( item );

		zSTRING visualName = (Z item->visual_change).Upper().Replace( ".ASC", ".MDM" );
		InsertSoftSkin( visualName, wear );
	}



	void oCNpc::InsertSoftSkin( const zSTRING& visualName, const int& wear ) {
		zCModelMeshLib* meshLib = new zCModelMeshLib();
		zCModelMeshLib::LoadMDM( visualName, Null, GetModel(), &meshLib );

		auto& softSkinList = meshLib->meshSoftSkinList;
		for( int i = 0; i < softSkinList.GetNum(); i++ )
			InsertSoftSkin( softSkinList[i], wear );

		meshLib->Release();
	}



	void oCNpc::InsertSoftSkin( zCMeshSoftSkin* softSkin, const int& wear ) {
		softSkin->AddRef();
		softSkin->objectName = SetWearSuffix( softSkin->objectName, wear );
		GetModel()->meshSoftSkinList.Insert( softSkin );
	}



	void oCNpc::RemoveSoftSkinItem( oCItem* item ) {
		if( !item )
			return;

		int wear = GetFirstWearSlot( item->wear );
		zSTRING slotName = SlotPrefix + wear;
		TNpcSlot* slot = CheckInvSlot( slotName );
		slot->ClearVob();
		RemoveSoftSkin( wear );
	}



	void oCNpc::RemoveSoftSkin( const int& wear ) {
		auto& softSkinList = GetModel()->meshSoftSkinList;
		zSTRING swear = wear;

		for( int i = 0; i < softSkinList.GetNum(); i++ ) {
			zCMeshSoftSkin* softSkin = softSkinList[i];
			zSTRING& softSkinName = softSkin->objectName;

			if( softSkinName.GetWord( SoftSkinWearSeparator, 2 ) == swear ) {
				softSkinList.RemoveOrderIndex( i-- );
				softSkin->Release();
			}
		}
	}



	HOOK Hook_oCNpc_EquipArmor AS( &oCNpc::EquipArmor, &oCNpc::EquipArmor_Union );

	void oCNpc::EquipArmor_Union( oCItem* item ) {
		if( item->HasFlag( ITM_FLAG_ACTIVE ) ) {
			UnequipItem( item );
			return;
		}

		if( !CanUse( item ) && this == player ) {
			DisplayCannotUse();
			return;
		}

		bool isValidArmorPart = Item_IsArmorPart( item ) && !item->HasFlag( ITM_FLAG_ACTIVE );
		if( item ) {
			if( !CanEquipArmorPart( item ) ) {
				string errorMessage = CreateMessage_CantEquipArmorPart( GetEquippedArmor(), item );
				ogame->game_text->Printwin( errorMessage );
				return;
			}

			UnequipArmorByWear( item->wear );
		}

		THISCALL( Hook_oCNpc_EquipArmor )(item);

		if( isValidArmorPart ) {
			EquipItem( item );
			InsertSoftSkinItem( item );
		}
	}



	bool oCNpc::CanEquipArmorPart( oCItem* item ) {
		oCItem* armor = GetEquippedArmor();
		if( !armor )
			return true;

		if( item->wear & ITM_WEAR_TORSO )
			return true;

		int collision = armor->wear & item->wear;
		if( (uint&)collision > ITM_WEAR_LIGHT )
			return false;

		return true;
	}



	HOOK Hook_oCNpc_UnequipItem AS( &oCNpc::UnequipItem, &oCNpc::UnequipItem_Union );

	void oCNpc::UnequipItem_Union( oCItem* item ) {
		bool isValidArmorPart = Item_IsArmorPart( item ) && item->HasFlag( ITM_FLAG_ACTIVE );

		if( isValidArmorPart )
			RemoveSoftSkinItem( item );

		THISCALL( Hook_oCNpc_UnequipItem )(item);
	}



	void oCNpc::UnequipArmorByWear( const int& wear ) {
		if( (uint&)wear <= ITM_WEAR_LIGHT )
			return;

		Array<oCItem*> toUnequip;
		for( int i = 0; i < invSlot.GetNumInList(); i++ ) {
			TNpcSlot* slot = invSlot[i];
			zSTRING slotName = slot->name;

			if( slotName.StartWith( SlotPrefix ) ) {
				oCItem* item = slot->vob->CastTo<oCItem>();
				if( !item )
					continue;

				int collision = item->wear & wear;
				if( (uint&)collision > ITM_WEAR_LIGHT )
					toUnequip += item;
			}
		}

		oCItem* equipedArmor = GetEquippedArmor();
		if( equipedArmor && equipedArmor->wear & wear )
			toUnequip += equipedArmor;

		for( uint i = 0; i < toUnequip.GetNum(); i++ )
			UnequipItem_Union( toUnequip[i] );
	}



	HOOK Hook_oCNpc_Unarchive PATCH( &oCNpc::Unarchive, &oCNpc::Unarchive_Union );

	void oCNpc::Unarchive_Union( zCArchiver& ar ) {
		THISCALL( Hook_oCNpc_Unarchive )(ar);

		for( int i = 0; i < invSlot.GetNumInList(); i++ ) {
			TNpcSlot* slot = invSlot[i];
			zSTRING slotName = slot->name;

			if( slotName.StartWith( SlotPrefix ) ) {
				oCItem* item = slot->vob->CastTo<oCItem>();

				if( item ) {
					item->SetFlag( ITM_FLAG_ACTIVE );
					item->AddRef();
					slot->ClearVob();
					slot->inInventory = True;
					InsertSoftSkinItem( item );
					item->Release();
				}
			}
		}
	}
}