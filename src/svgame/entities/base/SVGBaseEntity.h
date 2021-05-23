/*
// LICENSE HERE.

//
// SVGBaseEntity.h
//
// Base entity class, where the fun begins. All entities are inherited from this,
// one way or the other :)
//
*/
#ifndef __SVGAME_ENTITIES_BASE_SVGBASEENTITY_H__
#define __SVGAME_ENTITIES_BASE_SVGBASEENTITY_H__

class SVGBaseEntity {
public:
    //
    // Function pointers for actual callbacks.
    //
    using ThinkCallbackPointer      = void(SVGBaseEntity::*)(void);
    using UseCallbackPointer        = void(SVGBaseEntity::*)(SVGBaseEntity* other, SVGBaseEntity* activator);
    using TouchCallbackPointer      = void(SVGBaseEntity::*)(SVGBaseEntity* self, SVGBaseEntity* other, cplane_t* plane, csurface_t* surf);
    using BlockedCallbackPointer    = void(SVGBaseEntity::*)(SVGBaseEntity* other);
    using TakeDamageCallbackPointer = void(SVGBaseEntity::*)(SVGBaseEntity* other, float kick, int32_t damage);
    using DieCallbackPointer        = void(SVGBaseEntity::*)(SVGBaseEntity* inflictor, SVGBaseEntity* attacker, int damage, const vec3_t& point);

    //
    // Constructor/Deconstructor.
    //
    SVGBaseEntity(Entity* svEntity);
    virtual ~SVGBaseEntity();


    //
    // Interface functions. 
    //
    virtual void Precache();    // Precaches data.
    virtual void Spawn();       // Spawns the entity.
    virtual void Respawn();     // Respawns the entity.
    virtual void PostSpawn();   // PostSpawning is for handling entity references, since they may not exist yet during a spawn period.
    virtual void Think();       // General entity thinking routine.


    //
    // Callback functions.
    //
    void Use(SVGBaseEntity* other, SVGBaseEntity* activator);
    void Die(SVGBaseEntity* inflictor, SVGBaseEntity* attacker, int damage, const vec3_t& point);
    void Blocked(SVGBaseEntity* other);
    void Touch(SVGBaseEntity* self, SVGBaseEntity* other, cplane_t* plane, csurface_t* surf);
    void TakeDamage(SVGBaseEntity* other, float kick, int32_t damage);

    //
    // Entity Get Functions.
    //
    // Return the bounding box absolute 'min' value.
    inline const vec3_t& GetAbsoluteMin() {
        return serverEntity->absMin;
    }

    // Return the bounding box absolute 'max' value.
    inline const vec3_t& GetAbsoluteMax() {
        return serverEntity->absMax;
    }
    
    // Return the 'activatorEntity' entity pointer.
    SVGBaseEntity* GetActivator() {
        return activatorEntity;
    }

    // Return the 'angles' value.
    inline const vec3_t& GetAngles() {
        return serverEntity->state.angles;
    }

    // Return the 'angularVelocity' value.
    inline const vec3_t& GetAngularVelocity() {
        return angularVelocity;
    }

    // Return the 'className' value.
    inline const char* GetClassName() {
        return serverEntity->className;
    }

    // Return the 'client' pointer.
    gclient_s* GetClient() {
        return serverEntity->client;
    }

    // Return the 'clipmask' value.
    inline const int32_t GetClipMask() {
        return serverEntity->clipMask;
    }

    // Return the 'damage' value.
    inline const int32_t GetDamage() {
        return serverEntity->damage;
    }

    // Get the 'deadFlag' value.
    inline const int32_t GetDeadFlag() {
        return deadFlag;
    }

    // Return the 'delay' value.
    inline const int32_t GetDelay() {
        return serverEntity->delay;
    }

    // Return the 'effects' value.
    inline const uint32_t GetEffects() {
        return serverEntity->state.effects;
    }

    // Return the 'enemyPtr' entity pointer.
    SVGBaseEntity* GetEnemy() {
        return enemyEntity;
    }

    // Return the 'eventID' value.
    inline const uint8_t GetEventID() {
        return serverEntity->state.eventID;
    }

    // Return the 'flags' value.
    inline const int32_t GetFlags() {
        return serverEntity->flags;
    }

    // Return the 'frame' value.
    inline const int32_t GetFrame() {
        return serverEntity->state.frame;
    }

    // Return the 'groundEntitPtr' entity.
    inline SVGBaseEntity* GetGroundEntity() {
        return groundEntity;
    }

    // Return the 'groundEntityLinkCount' value.
    inline int32_t GetGroundEntityLinkCount() {
        return groundEntityLinkCount;
    }

    // Return the 'health' value.
    inline const int32_t GetHealth() {
        return health;
    }

    // Get the 'inuse' value.
    inline qboolean IsInUse() {
        return serverEntity->inUse;
    }

    // Get the 'killTarget' entity value.
    inline char* GetKillTarget() {
        return serverEntity->killTarget;
    }

    // Get the 'linkCount' value.
    inline const int32_t GetLinkCount() {
        return serverEntity->linkCount;
    }

    // Return the 'mass' value.
    inline const int32_t GetMass() {
        return mass;
    }

    // Return the 'maxHealth' value.
    inline const int32_t GetMaxHealth() {
        return maxHealth;
    }

    // Return the bounding box 'maxs' value.
    inline const vec3_t& GetMaxs() {
        return serverEntity->maxs;
    }

    // Return the 'message' value.
    inline const char* GetMessage() {
        return serverEntity->message;
    }

    // Return the bounding box 'mins' value.
    inline const vec3_t& GetMins() {
        return serverEntity->mins;
    }
   
    // Return the 'model' value.
    inline const char* GetModel() {
        return serverEntity->model;
    }

    // Return the 'modelIndex, modelIndex1, modelIndex2, modelIndex3' values.
    inline const int32_t GetModelIndex() {
        return serverEntity->state.modelIndex;
    }
    inline const int32_t GetModelIndex2() {
        return serverEntity->state.modelIndex2;
    }
    inline const int32_t GetModelIndex3() {
        return serverEntity->state.modelIndex3;
    }
    inline const int32_t GetModelIndex4() {
        return serverEntity->state.modelIndex4;
    }

    // Return the 'movetype' value.
    inline const int32_t GetMoveType() {
        return moveType;
    }

    // Return the 'nextThinkTime' value.
    inline const int32_t GetNextThinkTime() {
        return serverEntity->nextThinkTime;
    }

    // Return the 'noiseIndex' value.
    inline const int32_t GetNoiseIndex() {
        return serverEntity->noiseIndex;
    }

    // Return the 'noiseIndex2' value.
    inline const int32_t GetNoiseIndex2() {
        return serverEntity->noiseIndex2;
    }

    inline const int32_t GetNumber() {
        return serverEntity->state.number;
    }

    // Return the 'oldEnemyPtr' entity pointer.
    SVGBaseEntity* GetOldEnemy() {
        return oldEnemyEntity;
    }

    // Return the 'oldOrigin' value.
    inline const vec3_t& GetOldOrigin() {
        return serverEntity->state.oldOrigin;
    }

    // Return the 'origin' value.
    inline const vec3_t &GetOrigin() {
        return serverEntity->state.origin;
    }

    // Return the 'renderEffects' value.
    inline const int32_t SetRenderEffects() {
        return serverEntity->state.renderEffects;
    }

    // Set the 'pathTarget' entity value.
    inline char* GetPathTarget() {
        return serverEntity->pathTarget;
    }

    // Return the 'serverFlags' value.
    inline const int32_t GetServerFlags() {
        return serverEntity->serverFlags;
    }

    // Return the 'skinNumber' value.
    inline const int32_t GetSkinNumber() {
        return serverEntity->state.skinNumber;
    }

    // Return the 'size' value.
    inline const vec3_t& GetSize() {
        return serverEntity->size;
    }

    // Return the 'solid' value.
    inline const uint32_t GetSolid() {
        return serverEntity->solid;
    }

    // Return the 'spawnFlags' value.
    inline const int32_t GetSpawnFlags() {
        return serverEntity->spawnFlags;
    }

    // Return the 'style' value.
    inline const int32_t GetStyle() {
        return serverEntity->style;
    }

    // Return the 'sound' value.
    inline const int32_t GetSound() {
        return serverEntity->state.sound;
    }

    // Return the 'takeDamage' value.
    inline const int32_t GetTakeDamage() {
        return serverEntity->takeDamage;
    }

    // Return the 'target' entity value.
    inline char* GetTarget() {
        return serverEntity->target;
    }
    // Return the 'targetName' entity value.
    inline const char* GetTargetName() {
        return serverEntity->targetName;
    }

    // Return the 'team' entity value.
    inline char* GetTeam() {
        return serverEntity->team;
    }

    // Return the 'teamChain' entity value.
    inline SVGBaseEntity* GetTeamChainEntity() {
        return teamChainEntity;
    }

    // Return the 'teamMaster' entity value.
    inline SVGBaseEntity *GetTeamMasterEntity() {
        return teamMasterEntity;
    }

    // Return the 'viewHeight' entity value.
    inline const int32_t GetViewHeight() {
        return serverEntity->viewHeight;
    }

    // Return the 'velocity' value.
    inline const vec3_t& GetVelocity() {
        return velocity;
    }

    // Return the 'waterLevel' value.
    inline const int32_t GetWaterLevel() {
        return serverEntity->waterLevel;
    }

    // Return the 'waterType' value.
    inline const int32_t GetWaterType() {
        return serverEntity->waterType;
    }

    //
    // Entity Set Functions.
    //
    // Set the 'activatorEntity' pointer.
    inline void SetActivator(SVGBaseEntity* activator) {
        this->activatorEntity = activator;
    }
    
    // Return the 'angles' value.
    inline void SetAngles(const vec3_t& angles) {
        serverEntity->state.angles = angles;
    }

    // Set the 'angularVelocity' value.
    inline void SetAngularVelocity(const vec3_t& angularVelocity) {
        this->angularVelocity = angularVelocity;
    }

    // Set the 'mins', and 'maxs' values of the entity bounding box.
    inline void SetBoundingBox(const vec3_t& mins, const vec3_t& maxs) {
        serverEntity->mins = mins;
        serverEntity->maxs = maxs;
    }

    // Return the 'clipmask' value.
    inline void SetClipMask(const int32_t &clipMask) {
        serverEntity->clipMask = clipMask;
    }

    // Set the 'damage' value.
    inline void SetDamage(const int32_t &damage) {
        serverEntity->damage = damage;
    }

    // Set the 'deadFlag' value.
    inline void SetDeadFlag(const int32_t& deadFlag) {
        this->deadFlag = deadFlag;
    }

    // Set the 'effects' value.
    inline void SetEffects(const uint32_t &effects) {
        serverEntity->state.effects = effects;
    }

    // Return the 'delay' value.
    inline const int32_t SetDelay(const int32_t &delay) {
        serverEntity->delay = delay;
    }

    // Set the 'enemyPtr' pointer.
    inline void SetEnemy(SVGBaseEntity* enemy) {
        this->enemyEntity = enemy;
    }

    // Return the 'eventID' value.
    inline void SetEventID(const uint8_t &eventID) {
        serverEntity->state.eventID = eventID;
    }

    // Set the 'flags' value.
    inline void SetFlags(const int32_t &flags) {
        serverEntity->flags = flags;
    }

    // Set the 'frame' value.
    inline void SetFrame(const int32_t &frame) {
        serverEntity->state.frame = frame;;
    }

    // Set the 'groundEntitPtr' entity.
    inline void SetGroundEntity(SVGBaseEntity* groundEntity) {
        // Ensure to set the serverEntity its ground entity too.
        //
        // This is so sharedgame code can utilize it as well.
        //
        // TODO: Needs a lookup function for these entities in the
        // shared game code.
        if (groundEntity) {
            // Set the server entity side ground pointer.
            serverEntity->groundEntityPtr = groundEntity->GetServerEntity();

            // Set SVGBaseEntity variant ground entity.
            this->groundEntity = groundEntity;
        }
    }

    // Set the 'groundEntityLinkCount' value.
    inline void SetGroundEntityLinkCount(int32_t groundEntityLinkCount) {
        // Set it for THIS class entity.
        this->groundEntityLinkCount = groundEntityLinkCount;

        // Ensure it is also set on our server entity.
        serverEntity->groundEntityLinkCount = groundEntityLinkCount;
    }

    // Set the 'health' value.
    inline void SetHealth(const int32_t &health) {
        this->health = health;
    }

    // Set the 'inuse' value.
    inline void SetInUse(const qboolean& inUse) {
        serverEntity->inUse = inUse;
    }

    // Set the 'linkCount' value.
    inline void SetLinkCount(const int32_t &linkCount) {
        serverEntity->linkCount = linkCount;
    }

    // Set the 'mass' value.
    inline void SetMass(const int32_t &mass) {
        this->mass = mass;
    }

    // Set the 'maxHealth' value.
    inline void SetMaxHealth(const int32_t& maxHealth) {
        this->maxHealth = maxHealth;
    }

    // Set the 'maxs' value.
    inline void SetMaxs(const vec3_t& maxs) {
        serverEntity->maxs = maxs;
    }
    
    // Set the 'mins' value.
    inline void SetMins(const vec3_t& mins) {
        serverEntity->mins = mins;
    }

    // Set the 'modelIndex, modelIndex1, modelIndex2, modelIndex3' values.
    inline void SetModelIndex(const int32_t& index) {
        serverEntity->state.modelIndex = index;
    }
    inline void SetModelIndex2(const int32_t& index) {
        serverEntity->state.modelIndex2 = index;
    }
    inline void SetModelIndex3(const int32_t& index) {
        serverEntity->state.modelIndex3 = index;
    }
    inline void SetModelIndex4(const int32_t& index) {
        serverEntity->state.modelIndex4 = index;
    }

    // Set the 'model' value.
    inline void SetModel(const char* model) {
        // Set model.
        serverEntity->model = model;

        // Set model index.
        SetModelIndex(gi.ModelIndex(GetModel()));
    }

    // Set the 'nextThinkTime' value.
    inline void SetMoveType(const int32_t &moveType) {
        this->moveType = moveType;
    }

    // Set the 'nextThinkTime' value.
    inline void SetNextThinkTime(const float& nextThinkTime) {
        serverEntity->nextThinkTime = nextThinkTime;
    }
    
    // Set the 'oldEnemyPtr' pointer.
    inline void SetOldEnemy(SVGBaseEntity* oldEnemy) {
        this->oldEnemyEntity = oldEnemy;
    }

    // Set the 'origin' value.
    inline void SetOldOrigin(const vec3_t& oldOrigin) {
        serverEntity->state.oldOrigin = oldOrigin;
    }

    // Set the 'origin' value.
    inline void SetOrigin(const vec3_t& origin) {
        serverEntity->state.origin = origin;
    }

    // Set the 'renderEffects' value.
    inline void SetRenderEffects(const int32_t& renderEffects) {
        serverEntity->state.renderEffects = renderEffects;
    }

    // Set the 'serverFlags' value.
    inline void SetServerFlags(const int32_t &serverFlags) {
        serverEntity->serverFlags = serverFlags;
    }

    // Set the 'skinNumber' value.
    inline void SetSkinNumber(const int32_t& skinNumber) {
        serverEntity->state.skinNumber = skinNumber;
    }

    // Set the 'solid' value.
    inline void SetSolid(const uint32_t &solid) {
        serverEntity->solid = solid;
    }

    // Sets the 'sound' value.
    inline void SetSound(const int32_t& sound) {
        serverEntity->state.sound = sound;
    }

    // Set the 'spawnFlags' value.
    inline void SetSpawnFlags(const int32_t& spawnFlags) {
        serverEntity->spawnFlags = spawnFlags;
    }

    // Set the 'style' value.
    inline const uint32_t SetStyle(const int32_t &style) {
        serverEntity->style = style;
    }

    // Set the 'takeDamage' value.
    inline void SetTakeDamage(const int32_t& takeDamage) {
        serverEntity->takeDamage = takeDamage;
    }

    // Set the 'teamChain' entity value.
    inline void SetTeamChainEntity(SVGBaseEntity* entity) {
        teamChainEntity = entity;
    }

    // Set the 'teamMaster' entity value.
    inline void SetTeamMasterEntity(SVGBaseEntity* entity) {
        teamMasterEntity = entity;
    }

    // Set the 'viewHeight' entity value.
    inline void SetViewHeight(const int32_t& height) {
        serverEntity->viewHeight = height;
    }

    // Set the 'velocity' value.
    inline void SetVelocity(const vec3_t &velocity) {
        this->velocity = velocity;
    }

    // Return the 'waterLevel' value.
    inline void SetWaterLevel(const int32_t &waterLevel) {
        serverEntity->waterLevel = waterLevel;
    }

    // Return the 'waterType' value.
    inline void SetWaterType(const int32_t &waterType) {
        serverEntity->waterType = waterType;
    }


    //
    // General Entity Functions.
    //
    // Link entity to world for collision testing using gi.LinkEntity.
    void LinkEntity();

    // Returns the server entity pointer.
    inline Entity* GetServerEntity() {
        return serverEntity;
    }


protected:
    //
    // The actual entity this class is a member of.
    //
    Entity *serverEntity;

    //
    // Other base entity members. (These were old fields in edict_T back in the day.)
    //
    // Move Type. (MoveType:: ... )
    int32_t moveType;

    // Velocity.
    vec3_t velocity;
    // Angular Velocity.
    vec3_t angularVelocity;
    // Mass
    int32_t mass;

    // Ground Entity link count. (To keep track if it is linked or not.)
    int32_t groundEntityLinkCount;

    // Current health.
    int32_t health;
    // Maximum health.
    int32_t maxHealth;

    // Dead Flag. (Are we dead, dying or...?)
    int32_t deadFlag;

    //
    // Entity pointers.
    // 
    // Entity that activated this entity, NULL if none.
    SVGBaseEntity* activatorEntity;
    // Current active enemy, NULL if not any.    
    SVGBaseEntity *enemyEntity;
    // Ground entity we're standing on.
    SVGBaseEntity *groundEntity;
    // Old enemy, NULL if not any.
    SVGBaseEntity *oldEnemyEntity;
    // Team Chain Pointer.
    SVGBaseEntity* teamChainEntity;
    // Master Pointer.
    SVGBaseEntity* teamMasterEntity;
    
public:
    //
    // Ugly, but effective callback SET methods.
    //
    // Sets the 'Think' callback function.
    template<typename function>
    inline void SetThinkCallback(function f)
    {
        thinkFunction = static_cast<ThinkCallbackPointer>(f);
    }

    // Sets the 'Use' callback function.
    template<typename function>
    inline void SetUseCallback(function f)
    {
        useFunction = static_cast<UseCallbackPointer>(f);
    }

    // Sets the 'Touch' callback function.
    template<typename function>
    inline void SetTouchCallback(function f)
    {
        touchFunction = static_cast<TouchCallbackPointer>(f);
    }
    inline qboolean HasTouchCallback() {
        return (touchFunction != nullptr ? true : false);
    }

    // Sets the 'Blocked' callback function.
    template<typename function>
    inline void SetBlockedCallback(function f)
    {
        blockedFunction = static_cast<BlockedCallbackPointer>(f);
    }

    // Sets the 'SetTakeDamage' callback function.
    template<typename function>
    inline void SetTakeDamageCallback(function f)
    {
        takeDamageFunction = static_cast<TakeDamageCallbackPointer>(f);
    }
    inline qboolean HasTakeDamageCallback() {
        return (takeDamageFunction != nullptr ? true : false);
    }

    // Sets the 'Die' callback function.
    template<typename function>
    inline void SetDieCallback(function f)
    {
        dieFunction = static_cast<DieCallbackPointer>(f);
    }


protected:
    //
    // Callback function pointers.
    //
    ThinkCallbackPointer        thinkFunction;
    UseCallbackPointer          useFunction;
    TouchCallbackPointer        touchFunction;
    BlockedCallbackPointer      blockedFunction;
    TakeDamageCallbackPointer   takeDamageFunction;
    DieCallbackPointer          dieFunction;
};

#endif // __SVGAME_ENTITIES_BASE_CBASEENTITY_H__