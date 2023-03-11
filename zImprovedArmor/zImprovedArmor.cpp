// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
#if 0
	TNpcSlot* oCNpc::CheckInvSlot( const zSTRING& name ) {
		TNpcSlot* slot = GetInvSlot( name );
		if( slot )
			return slot;

		cmd << "New slot: " << name << endl;
		CreateInvSlot( name );
		return GetInvSlot( name );
	}



	Array<oCItem*> oCNpc::GetEquippedArmors() {
		Array<oCItem*> array;
#if ENGINE >= Engine_G2
		auto contList = inventory2.inventory.next;
#else
		auto contList = inventory2.inventory[INV_ARMOR].next;
#endif
		while( contList ) {
			oCItem* item = contList->data;
			if( item->HasFlag( ITM_CAT_ARMOR ) && item->HasFlag( ITM_FLAG_ACTIVE ) )
				array.Insert( item );
			contList = contList->next;
		}
		return array;
	}



  HOOK Hook_oCNpc_InitModel PATCH( &oCNpc::InitModel, &oCNpc::InitModel_Union );

	void oCNpc::InitModel_Union() {
#if 1
		zCModel* model = GetModel();
		if( !model ) {

			model->RemoveMeshLibAll();
			Array<oCItem*> armors = GetEquippedArmors();
			if( armors.GetNum() > 0 ) {
				for each( oCItem * armor in armors ) {
					zSTRING visualName = armor->visual_change;
					if( !visualName.IsEmpty() ) {
						model->ApplyMeshLib( visualName );
						model->SetMeshLibTexture( visualName, 0, body_TexVarNr, &zSTRING( "BODY" ) );
						model->SetMeshLibTexture( visualName, 1, body_TexColorNr, &zSTRING( "BODY" ) );
						model->SetMeshLibTexture( visualName, 0, armor->visual_skin, &zSTRING( "ARMOR" ) );
					}
				}
			}

			oCItem* bodyArmor = GetEquippedArmor();
			if( !bodyArmor ) {
				if( !body_visualName.IsEmpty() ) {
					model->ApplyMeshLib( body_visualName );
					model->SetMeshLibTexture( body_visualName, 0, body_TexVarNr, &zSTRING( "BODY" ) );
					model->SetMeshLibTexture( body_visualName, 1, body_TexColorNr, &zSTRING( "BODY" ) );
					model->SetMeshLibTexture( body_visualName, 0, 0, &zSTRING( "ARMOR" ) );
				}
			}

			SetHead();
			showVisual = True;
			zCModelPrototype* proto = model->modelProtoList[0];
			selfDist = proto->bbox3D.maxs[VZ] - proto->rootNodeTrans[VZ];
			if( selfDist <= 0 )
				selfDist = 150;

			// auto world = homeWorld;
			// if( world ) {
			// 	// world->RemoveVob( this );
			// 	// world->AddVob( this );
			// 	this->ThisVobRemovedFromWorld( world );
			// 	this->ThisVobAddedToWorld( world );
			// }

			//if( homeWorld )
			//	Enable( GetPositionWorld() );
		}
		THISCALL( Hook_oCNpc_InitModel )();

#else
		zCModel* model = GetModel();
		if( model ) {
			model->RemoveMeshLibAll();
			Array<oCItem*> armors = GetEquippedArmors();
			oCItem* bodyArmor = GetEquippedArmor();
			if( armors.GetNum() > 0 ) {
				for each( oCItem * armor in armors ) {
					if( armor == bodyArmor )
						continue;

					zSTRING visualName = armor->visual_change;
					if( visualName.IsEmpty() )
						continue;

					model->ApplyMeshLib( visualName );
					model->SetMeshLibTexture( visualName, 0, body_TexVarNr, &zSTRING( "BODY" ) );
					model->SetMeshLibTexture( visualName, 1, body_TexColorNr, &zSTRING( "BODY" ) );
					model->SetMeshLibTexture( visualName, 0, armor->visual_skin, &zSTRING( "ARMOR" ) );
				}
			}
		}
		THISCALL( Hook_oCNpc_InitModel )();
#endif
	}



	void oCNpc::InsertSoftSkinItem( oCItem* item ) {
		if( !item )
			return;

		int wear = GetFirstWearSlot( item->wear );
		zSTRING slotName = SlotPrefix + wear;
		TNpcSlot* slot = CheckInvSlot( slotName );

		if( Union.Dx11IsEnabled() ) {
			cmd << slot->name << endl;
			PutInSlot( slot, item, True );
			InitModel();
		}
		else {
			slot->SetVob( item );
			zSTRING visualName = (Z item->visual_change).Upper().Replace( ".ASC", ".MDM" );
			InsertSoftSkin( visualName, wear );
		}
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
		cmd << softSkin->objectName << endl;
		GetModel()->meshSoftSkinList.Insert( softSkin );
	}



	void oCNpc::RemoveSoftSkinItem( oCItem* item ) {
		if( !item )
			return;

		int wear = GetFirstWearSlot( item->wear );
		zSTRING slotName = SlotPrefix + wear;
		TNpcSlot* slot = CheckInvSlot( slotName );
		if( Union.Dx11IsEnabled() ) {
#if ENGINE == Engine_G2A
			RemoveFromSlot( slot, 0, 1 );
#else
			RemoveFromSlot( slot, 0 );
#endif
			GetModel()->RemoveMeshLib( item->visual_change );
		}
		else {
			slot->ClearVob();
			RemoveSoftSkin( wear );
		}
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
#endif
	zSTRING GetSoftSkinSlotName( int wear ) {
		return string::Combine( "%s%i", SlotPrefix, wear );
	}



	zSTRING GetSoftSkinItemUniqueName( oCItem* item ) {
		return string::Combine( "%z_%i", item->GetInstanceName(), item->wear);
	}



	bool IsSoftSkinSlot( const zSTRING& name ) {
		return name.StartWith( SlotPrefix );
	}
	
	
	
	bool IsAdditionalArmorItem( oCItem* item ) {
		return item && item->wear > ITM_WEAR_LIGHT && item->HasFlag( ITM_CAT_ARMOR );
	}



	// Array<oCItem*> oCNpc::GetEquippedArmors() {
	// 	Array<oCItem*> array;
	// 	for( int i = 0; i < invSlot.GetNum(); i++ ) {
	// 		auto slot = invSlot[i];
	// 		if( IsSoftSkinSlot( slot->name ) ) {
	// 			cmd << "  " << slot->name << "  " << AHEX32( slot->vob ) << endl;
	// 			if( slot->vob && slot->vob->type == zTVobType::zVOB_TYPE_ITEM )
	// 				array.Insert( (oCItem*)slot->vob );
	// 		}
	// 	}
	// 	return array;
	// }



	Array<oCItem*> oCNpc::GetEquippedArmors() {
		Array<oCItem*> array;
#if ENGINE >= Engine_G2
		auto contList = inventory2.inventory.next;
#else
		auto contList = inventory2.inventory[INV_ARMOR].next;
#endif
		while( contList ) {
			oCItem* item = contList->data;
			if( item->HasFlag( ITM_CAT_ARMOR ) && item->HasFlag( ITM_FLAG_ACTIVE ) )
				array.Insert( item );
			contList = contList->next;
		}
		return array;
	}



	HOOK Hook_oCNpc_InitModel PATCH( &oCNpc::InitModel, &oCNpc::InitModel_Union );

	void oCNpc::InitModel_Union() {
		zCModel* model = GetModel();
		if( !model ) {
			THISCALL( Hook_oCNpc_InitModel )();
			return;
		}

    model->RemoveMeshLibAll_Union(); // TODO
    Array<oCItem*> armors = GetEquippedArmors();
		oCItem* bodyArmor = GetEquippedArmor();
		if( !bodyArmor ) {
			if( !body_visualName.IsEmpty() ) {
				model->ApplyMeshLib_Union( body_visualName );
				model->SetMeshLibTexture_Union( body_visualName, 0, body_TexVarNr, &zSTRING( "BODY" ) );
				model->SetMeshLibTexture_Union( body_visualName, 1, body_TexColorNr, &zSTRING( "BODY" ) );
				model->SetMeshLibTexture_Union( body_visualName, 0, 0, &zSTRING( "ARMOR" ) );
			}
		}
		else
			armors.Insert( bodyArmor );

    for each( oCItem* armor in armors )
      InsertAdditionalArmorItemToMeshLib( armor );

    SetHead();
    showVisual = True;
    zCModelPrototype* proto = model->modelProtoList[0];
    selfDist = proto->bbox3D.maxs[VZ] - proto->rootNodeTrans[VZ];
    if( selfDist <= 0 )
      selfDist = 150;

    THISCALL( Hook_oCNpc_InitModel )();
		Enable( GetPositionWorld() );
	}



	TNpcSlot* oCNpc::CreateAdditionalItemSlot( const zSTRING& name ) {
		TNpcSlot* slot = GetInvSlot( name );
		if( slot )
			return slot;

		CreateInvSlot( name );
		return GetInvSlot( name );
	}



	HOOK Hook_oCNpc_UnequipItem AS( &oCNpc::UnequipItem, &oCNpc::UnequipItem_Union );
	
	void oCNpc::UnequipItem_Union( oCItem* item ) {
		bool isAdditionalArmor = IsAdditionalArmorItem( item ) && item->HasFlag( ITM_FLAG_ACTIVE );
		if( isAdditionalArmor )
			RemoveAdditionalArmorItemFromMeshLib( item );
	
		THISCALL( Hook_oCNpc_UnequipItem )(item);
	}



	void oCNpc::InsertAdditionalArmorItemToMeshLib( oCItem* item ) {
		if( !item )
			return;

		zCModel* model = GetModel();
		if( !model )
			return;

		zSTRING visualName = item->GetVisualChange();
		if( !visualName.IsEmpty() ) {
			model->ApplyMeshLib_Union( visualName );
			model->SetMeshLibTexture_Union( visualName, 0, body_TexVarNr, &zSTRING( "BODY" ) );
			model->SetMeshLibTexture_Union( visualName, 1, body_TexColorNr, &zSTRING( "BODY" ) );
			model->SetMeshLibTexture_Union( visualName, 0, item->visual_skin, &zSTRING( "ARMOR" ) );
		}
	}



	void oCNpc::RemoveAdditionalArmorItemFromMeshLib( oCItem* item ) {
		if( !item )
			return;

		zCModel* model = GetModel();
		if( !model )
			return;

		zSTRING visualName = item->GetVisualChange();
		if( !visualName.IsEmpty() )
			model->RemoveMeshLib_Union( visualName );
	}



	void oCNpc::PutAdditionalArmorItemToSlot( oCItem* item ) {
		if( !item )
			return;

		zCModel* model = GetModel();
		zSTRING slotName = GetSoftSkinSlotName( item->wear );
		TNpcSlot* slot = CreateAdditionalItemSlot( slotName );
		PutInSlot( slot, item, True );
	}



	void oCNpc::RemoveAdditionalArmorItemFromSlot( oCItem* item ) {
		if( !item )
			return;

		zCModel* model = GetModel();
		zSTRING slotName = GetSoftSkinSlotName( item->wear );
		TNpcSlot* slot = GetInvSlot( slotName );
		if( !slot )
			return;

		oCVob* vob = RemoveFromSlot( slot, 0, 1 );
		if( vob != item ) {
			// wtf ???
			return;
		}

		if( !Union.Dx11IsEnabled() )
			RemoveAdditionalArmorItemFromMeshLib( item );
		else
			InitModel_Union();
	}



	void oCNpc::EquipAdditionalArmorItem( oCItem* item ) {
		EquipItem( item );
		PutAdditionalArmorItemToSlot( item );
		
		if( !Union.Dx11IsEnabled() )
			InsertAdditionalArmorItemToMeshLib( item );
		else
			InitModel_Union();
	}



	HOOK Hook_oCNpc_EquipArmor AS_IF( &oCNpc::EquipArmor, &oCNpc::EquipArmor_Union, 1 );

	void oCNpc::EquipArmor_Union( oCItem* item ) {
		if( !item )
			return;

		if( item->HasFlag( ITM_FLAG_ACTIVE ) ) {
			UnequipItem( item );
			return;
		}

		if( !CanUse( item ) && this == player ) {
			DisplayCannotUse();
			return;
		}

		THISCALL( Hook_oCNpc_EquipArmor )(item);
		if( IsAdditionalArmorItem( item ) && !item->HasFlag( ITM_FLAG_ACTIVE ) )
			EquipAdditionalArmorItem( item );
	}



	HOOK Hook_oCNpc_Unarchive PATCH( &oCNpc::Unarchive, &oCNpc::Unarchive_Union );

	void oCNpc::Unarchive_Union( zCArchiver& ar ) {
		if( this == player )
		cmd << "--" << endl;
		THISCALL( Hook_oCNpc_Unarchive )(ar);

		// bool gd3d11 = Union.Dx11IsEnabled();
		for( int i = 0; i < invSlot.GetNumInList(); i++ ) {
			TNpcSlot* slot = invSlot[i];
			if( this == player )
				cmd << slot->name << endl;
			if( IsSoftSkinSlot( slot->name ) ) {
				oCItem* item = slot->vob->CastTo<oCItem>();

				if( item ) {
					if( this == player )
					cmd << item->name << endl;
					item->SetFlag( ITM_FLAG_ACTIVE );
					item->AddRef();
					// slot->ClearVob();
					slot->inInventory = True;
					// if( !gd3d11 )
						
					InsertAdditionalArmorItemToMeshLib( item );
				}
			}
		}

		// if( gd3d11 )
		// 	InitModel();
	}
}