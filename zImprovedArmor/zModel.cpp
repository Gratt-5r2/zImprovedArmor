// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// HOOK Hook_zCModel_RemoveMeshLibAll PATCH( &zCModel::RemoveMeshLibAll, &zCModel::RemoveMeshLibAll_Union );

	void zCModel::RemoveMeshLibAll_Union() {
		while( meshLibList.GetNum() )
			RemoveMeshLib_Union( meshLibList[0]->meshLib->GetObjectName() );
	}



	// HOOK Hook_zCModel_SetMeshLibTexture PATCH( &zCModel::SetMeshLibTexture, &zCModel::SetMeshLibTexture_Union );

	int zCModel::SetMeshLibTexture_Union( const zSTRING& meshLibName, const int channel, const int varNr, zSTRING* texNamePart ) {
		zSTRING nameNoExt = meshLibName.GetWord( '.' );
		int applies = 0;
		for( int i = 0; i < meshLibList.GetNum(); i++ ) {
			if( meshLibList[i]->meshLib->objectName == nameNoExt ) {
				meshLibList[i]->texAniState.SetChannelVariation( channel, varNr, texNamePart );
				applies++;
			}
		}
		return applies;
	}



	// HOOK Hook_zCModel_RemoveMeshLib PATCH( &zCModel::RemoveMeshLib, &zCModel::RemoveMeshLib_Union );

	int zCModel::RemoveMeshLib_Union( const zSTRING& meshLibName ) {
		int removes = 0;
		zSTRING nameNoExt = meshLibName.GetWord( '.' );

		for( int i = 0; i < meshLibList.GetNum(); i++ ) {
			auto meshLibListNode = meshLibList[i];
			if( meshLibListNode->meshLib->objectName == nameNoExt ) {
				zCModelMeshLib* meshLib = meshLibListNode->meshLib;
				delete meshLibListNode;
				meshLibList.RemoveOrderIndex( i-- );
				meshLib->RemoveFromModel( this );
				meshLib->Release();
				removes++;
			}
		}

		return False;
	}



	int zCModel::ApplyMeshLib_Union( const zSTRING& meshLibName ) {
		return ApplyMeshLib( meshLibName );

		int removes = 0;
		zSTRING nameNoExt = meshLibName.GetWord( '.' );

		// for( int i = 0; i < meshLibList.GetNum(); i++ ) {
		// 	auto meshLibListNode = meshLibList[i];
		// 	if( meshLibListNode->meshLib->objectName == nameNoExt ) {
		// 
		// 	}
		// }

		//zCModelMeshLib* meshLib = Null;
		//zCModelMeshLib::LoadMDM( meshLibName, Null, this, &meshLib );
		// if( meshLib == Null )
		//	return ApplyMeshLib( meshLibName );

		//zCModelMeshLib* meshLibUnique = new zCModelMeshLib();
		//for( int i = 0; i < meshLib->meshNodeList.GetNum(); i++ ) {
		//	auto node = meshLib->meshNodeList[i];
		//	meshLibUnique->AddNodeVisual( node.nodeIndex, node.visual );
		//}
		//meshLib->Release();
		  // zCModelMeshLib* meshLibUnique = meshLib;
		  // 
		  // 
		  // meshLibUnique->ApplyToModel( this );
		  // zTMeshLibEntry* meshLibEntry = new zTMeshLibEntry();
		  // meshLibEntry->meshLib = meshLibUnique;
		  // meshLibEntry->texAniState.BuildTexListFromMeshLib( meshLib );
		  // meshLibList.InsertEnd( meshLibEntry );

		/// if( ApplyMeshLib( meshLibName ) ) {
		/// 	meshLibList[meshLibList.GetNum() - 1]->meshLib->ApplyToModel( this );
		/// 	return True;
		/// }
		/// return False;
	}



  // HOOK Hook_zCModelMeshLib_LoadMDM PATCH( &zCModelMeshLib::LoadMDM, &zCModelMeshLib::LoadMDM_Single );
	
	int zCModelMeshLib::LoadMDM_Single( const zSTRING& fileName, zCModelPrototype* destProto,
		zCModel* destModel, zCModelMeshLib** resMeshLib ) {
		int ok = LoadMDM_Try( fileName, destProto, destModel, resMeshLib );
		if( ok && *resMeshLib )
			(*resMeshLib)->SetObjectName( string::Combine( "%z_%x", (*resMeshLib)->GetObjectName(), *resMeshLib ) );
		return ok;
	}
}