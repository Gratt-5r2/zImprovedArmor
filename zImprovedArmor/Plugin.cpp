// This file added in headers queue
// File: "Sources.h"

namespace GOTHIC_ENGINE {

  // TO DO
  // Your code ...
  
  void Game_Entry() {
  }
  
  void Game_Init() {
  }

  void Game_Exit() {
  }

  void Game_PreLoop() {
  }

  void ShowSoftSkin( oCNpc* npc, int x ) {
    if( !npc )
      return;
    zCModel* model = npc->GetModel();
    if( !model )
      return;

#if 0
    model->meshSoftSkinList.EmptyList();

    int counter = 0;
    for( int i = 0; i < model->meshLibList.GetNum(); i++ )
    {
      auto& meshLib = model->meshLibList[i];
      for( int j = 0; j < meshLib->meshLib->meshSoftSkinList.GetNum(); j++ )
      {
        auto* softSkin = meshLib->meshLib->meshSoftSkinList[j];
        model->meshSoftSkinList.Insert( softSkin );
        screen->Print( x, 1000 + screen->FontY() * counter++, softSkin->GetVisualName() );
      }
    }
#elif 0
    int counter = 0;
    for( int i = 0; i < model->meshSoftSkinList.GetNum(); i++ ) {
      screen->Print( x, 1000 + screen->FontY() * counter++, model->meshSoftSkinList[i]->GetVisualName());
    }
#elif 0
    {
      int counter = 0;
      for( int i = 0; i < model->meshLibList.GetNum(); i++ )
      {
        auto& meshLib = model->meshLibList[i];
        for( int j = 0; j < meshLib->meshLib->meshSoftSkinList.GetNum(); j++ )
        {
          auto* softSkin = meshLib->meshLib->meshSoftSkinList[j];
          screen->Print( x, 1000 + screen->FontY() * counter++, softSkin->GetVisualName() );
        }
      }
    }
    {
      int counter = 0;
      for( int i = 0; i < model->meshSoftSkinList.GetNum(); i++ ) {
        screen->Print( x + 1000, 1000 + screen->FontY() * counter++, model->meshSoftSkinList[i]->GetVisualName() );
      }
    }
#endif
  }

  void Game_Loop() {
    if( zKeyToggled( KEY_U ) ) {
      zCModel* model = player->GetModel();
      player->RefreshNpc();

      model->AddRef();
      player->SetVisual( Null );
      player->SetVisual( model );
      model->Release();

      cmd << "u" << endl;

      auto arc = zarcFactory->CreateArchiverWrite(zTArchiveMode::zARC_MODE_BINARY_SAFE, 0, 0);
      model->Archive( *arc );
      model->Unarchive( *arc );
    }


    ShowSoftSkin( player, 100 );
    ShowSoftSkin( player->GetFocusNpc(), 7000 );
  }

  void Game_PostLoop() {
  }

  void Game_MenuLoop() {
  }

  // Information about current saving or loading world
  TSaveLoadGameInfo& SaveLoadGameInfo = UnionCore::SaveLoadGameInfo;

  void Game_SaveBegin() {
  }

  void Game_SaveEnd() {
  }

  void LoadBegin() {
  }

  void LoadEnd() {
  }

  void Game_LoadBegin_NewGame() {
    LoadBegin();
  }

  void Game_LoadEnd_NewGame() {
    LoadEnd();
  }

  void Game_LoadBegin_SaveGame() {
    LoadBegin();
  }

  void Game_LoadEnd_SaveGame() {
    LoadEnd();
  }

  void Game_LoadBegin_ChangeLevel() {
    LoadBegin();
  }

  void Game_LoadEnd_ChangeLevel() {
    LoadEnd();
  }

  void Game_LoadBegin_Trigger() {
  }
  
  void Game_LoadEnd_Trigger() {
  }
  
  void Game_Pause() {
  }
  
  void Game_Unpause() {
  }



  class zCMeshSoftSkinIndexed : public zCMeshSoftSkin {
  public:
    int SlotID;
  };



  int Mdl_ApplySoftSkinItem() {
    oCItem* item = (oCItem*)parser->GetInstance();
    oCNpc* npc = (oCNpc*)parser->GetInstance();
    
    zCModel* model = player->GetModel();
    zSTRING visualName = (Z item->visual_change).Upper().Replace( ".ASC", ".MDM" );

    zCModelMeshLib* meshLib = new zCModelMeshLib();
    zCModelMeshLib::LoadMDM( visualName, Null, model, &meshLib );

    auto& softSkinList = meshLib->meshSoftSkinList;
    for( int i = 0; i < softSkinList.GetNum(); i++ ) {
      zCMeshSoftSkin* softSkin = softSkinList[0];
      softSkin->AddRef();
      model->meshSoftSkinList.Insert( softSkin );
    }
    
    meshLib->Release();
    return 0;
  }

  void Game_DefineExternals() {
    parser->DefineExternal( "Mdl_ApplySoftSkinItem", Mdl_ApplySoftSkinItem, zPAR_TYPE_VOID, zPAR_TYPE_INSTANCE, zPAR_TYPE_INSTANCE, 0 );
  }

  /*
  Functions call order on Game initialization:
    - Game_Entry           * Gothic entry point
    - Game_DefineExternals * Define external script functions
    - Game_Init            * After DAT files init
  
  Functions call order on Change level:
    - Game_LoadBegin_Trigger     * Entry in trigger
    - Game_LoadEnd_Trigger       *
    - Game_Loop                  * Frame call window
    - Game_LoadBegin_ChangeLevel * Load begin
    - Game_SaveBegin             * Save previous level information
    - Game_SaveEnd               *
    - Game_LoadEnd_ChangeLevel   *
  
  Functions call order on Save game:
    - Game_Pause     * Open menu
    - Game_Unpause   * Click on save
    - Game_Loop      * Frame call window
    - Game_SaveBegin * Save begin
    - Game_SaveEnd   *
  
  Functions call order on Load game:
    - Game_Pause              * Open menu
    - Game_Unpause            * Click on load
    - Game_LoadBegin_SaveGame * Load begin
    - Game_LoadEnd_SaveGame   *
  */

#define AppDefault True
  CApplication* lpApplication = !CHECK_THIS_ENGINE ? Null : CApplication::CreateRefApplication(
    Enabled( AppDefault ) Game_Entry,
    Enabled( AppDefault ) Game_Init,
    Enabled( AppDefault ) Game_Exit,
    Enabled( AppDefault ) Game_PreLoop,
    Enabled( AppDefault ) Game_Loop,
    Enabled( AppDefault ) Game_PostLoop,
    Enabled( AppDefault ) Game_MenuLoop,
    Enabled( AppDefault ) Game_SaveBegin,
    Enabled( AppDefault ) Game_SaveEnd,
    Enabled( AppDefault ) Game_LoadBegin_NewGame,
    Enabled( AppDefault ) Game_LoadEnd_NewGame,
    Enabled( AppDefault ) Game_LoadBegin_SaveGame,
    Enabled( AppDefault ) Game_LoadEnd_SaveGame,
    Enabled( AppDefault ) Game_LoadBegin_ChangeLevel,
    Enabled( AppDefault ) Game_LoadEnd_ChangeLevel,
    Enabled( AppDefault ) Game_LoadBegin_Trigger,
    Enabled( AppDefault ) Game_LoadEnd_Trigger,
    Enabled( AppDefault ) Game_Pause,
    Enabled( AppDefault ) Game_Unpause,
    Enabled( AppDefault ) Game_DefineExternals
  );
}