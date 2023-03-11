// Supported with union (c) 2020 Union team

// User API for zCModel
// Add your methods here

int SetMeshLibTexture_Union( const zSTRING& meshLibName, const int channel, const int varNr, zSTRING* texNamePart );
int RemoveMeshLib_Union( const zSTRING& meshLibName );
void RemoveMeshLibAll_Union();
int ApplyMeshLib_Union( zSTRING const& );