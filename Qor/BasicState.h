#ifndef _BASICSTATE_H_VZ3QNB09
#define _BASICSTATE_H_VZ3QNB09

#include "State.h"
#include "Input.h"
#include "TileMap.h"
#include "Camera.h"
#include "Pipeline.h"
#include "Mesh.h"
#include "Sprite.h"
#include "PlayerInterface2D.h"
#include "Light.h"
#include "Text.h"
#include "Menu.h"
#include "RenderBuffer.h"

class Qor;

class BasicState:
    public State
{
    public:
        BasicState(Qor* engine);
        virtual ~BasicState();

        virtual void preload() override;
        virtual void enter() override;
        virtual void logic(Freq::Time t) override;
        virtual void render() const override;
        virtual bool needs_load() const override {
            return true;
        }

    private:
        Qor* m_pQor = nullptr;
        Input* m_pInput = nullptr;
        Pipeline* m_pPipeline = nullptr;
        ResourceCache* m_pResources = nullptr;

        std::shared_ptr<Node> m_pRoot;
        std::shared_ptr<Node> m_pRTTRoot;
        //std::shared_ptr<Node> m_pTemp;
        std::shared_ptr<Sprite> m_pSprite;
        std::shared_ptr<PlayerInterface2D> m_pPlayer;
        std::shared_ptr<TileMap> m_pMap;
        std::shared_ptr<Camera> m_pCamera;
        std::shared_ptr<Camera> m_pRTTCamera;
        std::shared_ptr<Light> m_pLight;
        std::shared_ptr<RenderBuffer> m_pRenderBuffer;
        //unsigned m_DetailShader = ~0u;

        //unsigned m_Shader = ~0u;
};

#endif

