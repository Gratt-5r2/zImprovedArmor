// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	HOOK Hook_zCModel_SetMeshLibTexture PATCH( &zCModel::SetMeshLibTexture, &zCModel::SetMeshLibTexture_Union );

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



	HOOK Hook_zCModel_RemoveMeshLib PATCH( &zCModel::RemoveMeshLib, &zCModel::RemoveMeshLib_Union );

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
}