// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  zSTRING GetSoftSkinSlotName( int wear ) {
    return string::Combine( "%s%i", SlotPrefix, wear );
  }



  bool IsSoftSkinSlot( const zSTRING& name ) {
    return name.StartWith( SlotPrefix );
  }
  
  
  
  bool IsAdditionalArmorItem( oCItem* item ) {
    return item && (uint)item->wear > ITM_WEAR_LIGHT && item->HasFlag( ITM_CAT_ARMOR );
  }


#if 0
  Array<oCItem*> oCNpc::GetEquippedArmors() {
    Array<oCItem*> array;
    for( int i = 0; i < invSlot.GetNum(); i++ ) {
      auto slot = invSlot[i];
      if( IsSoftSkinSlot( slot->name ) ) {
        cmd << "  " << slot->name << "  " << AHEX32( slot->vob ) << endl;
        if( slot->vob && slot->vob->type == zTVobType::zVOB_TYPE_ITEM )
          array.Insert( (oCItem*)slot->vob );
      }
    }
    return array;
  }
#else
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
#endif


  HOOK Hook_oCNpc_InitModel PATCH( &oCNpc::InitModel, &oCNpc::InitModel_Union );

  void oCNpc::InitModel_Union() {
    zCModel* model = GetModel();
    if( !model ) {
      THISCALL( Hook_oCNpc_InitModel )();
      return;
    }

    model->RemoveMeshLibAll();
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

    if( Union.Dx11IsEnabled() )
      THISCALL( Hook_oCNpc_InitModel )();
  }



  TNpcSlot* oCNpc::CreateAdditionalItemSlot( const zSTRING& name ) {
    TNpcSlot* slot = GetInvSlot( name );
    if( slot )
      return slot;

    CreateInvSlot( name );
    return GetInvSlot( name );
  }



  void oCNpc::UnequipAdditionalArmorFromWear( int wear ) {
    if( (uint)wear <= ITM_WEAR_LIGHT )
      return;

    for( int i = 0; i < invSlot.GetNumInList(); i++ ) {
      TNpcSlot* slot = invSlot[i];
      zSTRING slotName = slot->name;

      if( slotName.StartWith( SlotPrefix ) ) {
        oCItem* item = slot->vob->CastTo<oCItem>();
        if( !item )
          continue;

        uint collision = item->wear & wear;
        if( collision > ITM_WEAR_LIGHT )
          UnequipItem_Union( item );
      }
    }
  }



  HOOK Hook_oCNpc_UnequipItem AS( &oCNpc::UnequipItem, &oCNpc::UnequipItem_Union );
  
  void oCNpc::UnequipItem_Union( oCItem* item ) {
    if( !item )
      return;

    if( IsAdditionalArmorItem( item ) && item->HasFlag( ITM_FLAG_ACTIVE ) ) {
      item->AddRef();
      THISCALL( Hook_oCNpc_UnequipItem )(item);
      RemoveAdditionalArmorItemFromSlot( item );
      item->Release();
    }
    else
      THISCALL( Hook_oCNpc_UnequipItem )(item);
  }



  void oCNpc::InsertAdditionalArmorItemToMeshLib( oCItem* item ) {
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
    zCModel* model = GetModel();
    zSTRING slotName = GetSoftSkinSlotName( item->wear );
    TNpcSlot* slot = GetInvSlot( slotName );
    if( !slot )
      return;

#if ENGINE == Engine_G2A
    oCVob* vob = RemoveFromSlot( slot, 0, 1 );
#else
    oCVob* vob = RemoveFromSlot( slot, 0 );
#endif
    if( vob != item ) {
      // wtf ???
      return;
    }

    RemoveAdditionalArmorItemFromMeshLib( item );
    if( homeWorld && Union.Dx11IsEnabled() )
      InitModel_Union();
  }



  bool oCNpc::CanEquipAdditionalArmor( oCItem* item ) {
    if( item->wear & ITM_WEAR_TORSO )
      return true;

    oCItem* armor = GetEquippedArmor();
    if( !armor )
      return true;

    return (armor->wear & item->wear) != item->wear;
  }



  void oCNpc::EquipAdditionalArmorItem( oCItem* item ) {
    EquipItem( item );
    PutAdditionalArmorItemToSlot( item );
    InsertAdditionalArmorItemToMeshLib( item );
    if( homeWorld && Union.Dx11IsEnabled() )
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

    if( !CanEquipAdditionalArmor( item ) ) {
      string errorMessage = CreateMessage_CantEquipArmorPart( GetEquippedArmor(), item );
      ogame->game_text->Printwin( errorMessage );
      return;
    }

    UnequipAdditionalArmorFromWear( item->wear );

    THISCALL( Hook_oCNpc_EquipArmor )(item);
    if( IsAdditionalArmorItem( item ) && !item->HasFlag( ITM_FLAG_ACTIVE ) )
      EquipAdditionalArmorItem( item );
  }



  HOOK Hook_oCNpc_Unarchive PATCH( &oCNpc::Unarchive, &oCNpc::Unarchive_Union );

  void oCNpc::Unarchive_Union( zCArchiver& ar ) {
    if( this == player )
    THISCALL( Hook_oCNpc_Unarchive )(ar);

    // bool gd3d11 = Union.Dx11IsEnabled();
    for( int i = 0; i < invSlot.GetNumInList(); i++ ) {
      TNpcSlot* slot = invSlot[i];
      if( this == player )
      if( IsSoftSkinSlot( slot->name ) ) {
        oCItem* item = slot->vob->CastTo<oCItem>();

        if( item ) {
          if( this == player )
          item->SetFlag( ITM_FLAG_ACTIVE );
          item->AddRef();
          slot->inInventory = True;            
          InsertAdditionalArmorItemToMeshLib( item );
        }
      }
    }

    if( Union.Dx11IsEnabled() )
      InitModel();
  }
}