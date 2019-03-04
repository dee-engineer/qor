#ifndef QOR_PYTHON_BINDINGS_H
#define QOR_PYTHON_BINDINGS_H
#if !defined(QOR_NO_PYTHON) && !defined(QOR_NO_SCRIPTING)

#include <cmath>
#include "kit/log/log.h"
#include "kit/log/errors.h"
#include "kit/freq/animation.h"
#include "kit/math/matrixops.h"
#include "kit/math/vectorops.h"
#include "../Node.h"
#include "../Mesh.h"
#include "../Graphics.h"
#include "../Window.h"
#include "../Qor.h"
#include "../Interpreter.h"
#include "../Sprite.h"
#include "../Sound.h"
#include "../TileMap.h"
#include "../Graphics.h"
#include "../Camera.h"
#include "../Light.h"
#include "../Particle.h"
#include "../NodeInterface.h"
#include "../PlayerInterface3D.h"
#include "../BasicPartitioner.h"
#include "../Headless.h"

namespace Scripting
{
    Qor* qor() {
        //return (Qor*)Interpreter::current()->user_data();
        return Qor::get();
    }

    //struct NodeGroup
    //{
    //    //std::vector
    //};
    
    void statemachine_on_tick(
        StateMachine& s,
        std::string slot,
        std::string state,
        boost::python::object cb
    ){
        s.on_tick(slot,state,[cb](Freq::Time t){ cb(t.s()); });
    }
    void statemachine_on_enter(
        StateMachine& s,
        std::string slot,
        std::string state,
        boost::python::object cb
    ){
        s.on_enter(slot,state,[cb](){ cb(); });
    }
    void statemachine_on_leave(
        StateMachine& s,
        std::string slot,
        std::string state,
        boost::python::object cb
    ){
        s.on_leave(slot,state,[cb](){ cb(); });
    }
    void statemachine_on_attempt(
        StateMachine& s,
        std::string slot,
        std::string state,
        boost::python::object cb
    ){
        s.on_attempt(slot,state,[cb](std::string state){
            return cb(state);
        });
    }
    void statemachine_on_reject(
        StateMachine& s,
        std::string slot,
        std::string state,
        boost::python::object cb
    ){
        s.on_reject(slot,state,[cb](std::string state){ cb(state); });
    }
    
    struct MetaBind
    {
        std::shared_ptr<Meta> m;

        MetaBind(): m(std::make_shared<Meta>()) {}
        MetaBind(const std::shared_ptr<Meta>& meta):
            m(meta)
        {}
        ~MetaBind() {}

        bool empty() const { return m->empty(); }
    };
    
    struct NodeBind
    {
        std::shared_ptr<Node> n;
        //std::weak_ptr<Node> p;
        NodeBind():
            n(std::make_shared<Node>())
        {}
        NodeBind(std::nullptr_t) {}
        NodeBind(Node* n):
            n(n ? n->as_node() : nullptr)
        {}
        NodeBind(const NodeBind& rhs) = default;
        NodeBind& operator=(const NodeBind& rhs) = default;
        explicit NodeBind(const std::shared_ptr<Node>& p):
            n(p)
        {}
        NodeBind as_node() {
            return NodeBind(n);
        }
        virtual ~NodeBind() {}
        void discard() {
            n->discard();
            n = nullptr;
        }
        void nullify() {
            n = nullptr;
        }

        void each(boost::python::object cb) {
            n->each([cb](Node* n){
                cb(NodeBind(n));
            });
        }
        
        void set_visible(bool b) {
            n->visible(b);
        }
        bool get_visible() const {
            return n->visible();
        }
        void set_self_visible(bool b) {
            n->self_visible(b);
        }
        bool get_self_visible() const {
            return n->self_visible();
        }

        void rotate(float turns, glm::vec3 v, Space s) {
            n->rotate(turns,v,s);
        }
        //void rescale(float f) { n->rescale(f); }
        void scale(float f) { n->scale(f); }
        boost::python::object get_matrix() const {
            boost::python::list l;
            const float* f = value_ptr(*n->matrix_c());
            for(unsigned i=0;i<16;++i)
                l.append<float>(f[i]);
            return l;
        }
        void set_matrix(boost::python::list m) {
            float* f = value_ptr(*n->matrix());
            for(unsigned i=0;i<16;++i)
                f[i] = boost::python::extract<float>(m[i]);
            pend();
        }
        //void set_position(glm::vec3 v, Space s = Space::PARENT) {
        void set_position(glm::vec3 v, Space s = Space::PARENT) {
            n->position(v,s);
        }
        glm::vec3 get_position(Space s = Space::PARENT) const {
            //glm::vec3 p = n->position();
            //boost::python::list l;
            //for(unsigned i=0;i<3;++i)
            //    l.append<float>(p[i]);
            //return l;
            return n->position(s);
        }
        glm::vec3 get_velocity() {return n->velocity();}
        void set_velocity(glm::vec3 v) {n->velocity(v);}
        void move(glm::vec3 v, Space s = Space::PARENT) {
            n->move(v);
            //n->position(n->position() + glm::vec3(
            //    boost::python::extract<float>(t[0]),
            //    boost::python::extract<float>(t[1]),
            //    boost::python::extract<float>(t[2])
            //));
        }
        void detach() { n->detach(); }
        void collapse(Space s) {n->collapse(s);}
        //virtual std::string type() const {
        //    return "node";
        //}
        void add(NodeBind nh) { n->add(nh.n); }
        void stick(NodeBind nh) { n->stick(nh.n); }
        void spawn() {
            detach();
            qor()->current_state()->root()->add(n);
        }
        bool valid() const { return !!n; }
        NodeBind parent() const { return NodeBind(n->parent()); }
        NodeBind subroot() const { return NodeBind(n->subroot());}
        NodeBind root() const { return NodeBind(n->root());}
        void pend() { n->pend(); }
        void add_tag(std::string tag) { n->add_tag(tag); }
        bool has_tag(std::string tag) { return n->has_tag(tag); }
        void remove_tag(std::string tag) { n->remove_tag(tag); }
        unsigned num_descendents() const { return n->num_descendents(); }
        unsigned num_children() const { return n->num_children(); }
        Box box() { return n->box(); }
        Box world_box() { return n->world_box(); }
        boost::python::list children() {
            boost::python::list l;
            auto ns = n->children();
            for(auto&& ch: ns)
                l.append<NodeBind>(NodeBind(std::move(ch)));
            return l;
        }

        boost::python::object find(std::string s) {
            boost::python::list l;
            auto ns = n->find(s, Node::Find::REGEX);
            for(auto&& n: ns)
                l.append<NodeBind>(NodeBind(std::move(n)));
            return l;
        }
        boost::python::object find_if(boost::python::object cb) {
            boost::python::list l;
            auto ns = n->find_if([cb](Node* n){
                return cb(NodeBind(n));
            });
            for(auto&& n: ns)
                l.append<NodeBind>(NodeBind(std::move(n)));
            return l;
        }
        boost::python::object find_type(std::string t) {
            boost::python::list l;
            auto ns = n->find_type(t);
            for(auto&& n: ns)
                l.append<NodeBind>(NodeBind(std::move(n)));
            return l;
        }

        void on_tick(boost::python::object cb){
            n->on_tick.connect([cb](Freq::Time t){
                cb(t.s());
            });
        }

        void event(std::string ev, MetaBind m){ n->event(ev,m.m); }
        void on_event(std::string ev, boost::python::object cb){
            n->event(ev,[cb](const std::shared_ptr<Meta>& m){
                cb(MetaBind(m));
            });
        }
        bool has_event(std::string ev) { return n->has_event(ev); }
        
        MetaBind config() {
            return MetaBind(n->config());
        }
#ifndef QOR_NO_PHYSICS
        void generate() {
            qor()->current_state()->physics()->generate(
                n.get(), Physics::GEN_RECURSIVE
            );
        }
#endif
        std::string state(std::string slot) const { return n->state(slot); }
        void set_state(std::string slot, std::string state) { return n->state(slot,state); }
        
        void on_state_tick(std::string slot, std::string state, boost::python::object cb){
            statemachine_on_tick(*n, slot, state, cb);
        };
        void on_state_enter(std::string slot, std::string state, boost::python::object cb){
            statemachine_on_enter(*n, slot, state, cb);
        };
        void on_state_leave(std::string slot, std::string state, boost::python::object cb){
            statemachine_on_leave(*n, slot, state, cb);
        };
        void on_state_attempt(std::string slot, std::string state, boost::python::object cb){
            statemachine_on_attempt(*n, slot, state, cb);
        };
        void on_state_reject(std::string slot, std::string state, boost::python::object cb){
            statemachine_on_reject(*n, slot, state, cb);
        };
        void clear_state(std::string slot) { n->StateMachine::clear(slot); }
        void clear_states() { n->StateMachine::clear(); }
        
        Node::Physics physics() const { return n->physics(); }
        Node::PhysicsShape physics_shape() const { return n->physics_shape(); }
    };

    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(node_set_position_overloads, set_position, 1, 2)
    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(node_get_position_overloads, get_position, 0, 1)

    struct MeshBind:
        public NodeBind
    {
        MeshBind():
            NodeBind(std::static_pointer_cast<Node>(std::make_shared<Mesh>()))
        {}
        explicit MeshBind(const std::shared_ptr<Mesh>& mesh):
            NodeBind(std::static_pointer_cast<Node>(mesh))
        {}
        explicit MeshBind(std::string fn):
            NodeBind(nullptr)
        {
            n = std::make_shared<Mesh>(
                qor()->resource_path(fn),
                qor()->resources()
            );
            //n = std::make_shared<Mesh>(qor()->nodes().create_as<Mesh::Data>(
            //    std::tuple<
            //        std::string,
            //        IFactory*,
            //        ICache*
            //    >(
            //        fn,
            //        &qor()->nodes(),
            //        qor()->resources()
            //    )
            //));
        }
        explicit MeshBind(NodeBind nb): NodeBind(nb) {}
        virtual ~MeshBind() {}
        Mesh* self() {
            return (Mesh*)n.get();
        }
        MeshBind instance() { return MeshBind(self()->instance()); }
        MeshBind prototype() { return MeshBind(self()->prototype()); }
        //virtual std::string type() const {
        //    return "mesh";
        //}
        void set_physics(Node::Physics ps) {self()->set_physics(ps);}
        void set_physics_shape(Node::PhysicsShape ps) {self()->set_physics_shape(ps);}

        float get_mass() { return self()->mass(); }
        void set_mass(float f) { self()->mass(f); }
        void friction(float f) { self()->friction(f); }

        void impulse(glm::vec3 a) { self()->impulse(a); }
        void gravity(glm::vec3 a) { self()->gravity(a); }
        void inertia(bool b) { self()->inertia(b); }
        void clear_body() { self()->clear_body(); }
        void teleport(glm::vec3 v) { self()->teleport(v); }

        void material(std::string fn) {
            self()->material(fn, qor()->resources());
        }
        void swap_material(std::string to, std::string from) {
            self()->swap_material(to, from, qor()->resources());
        }
    };

    struct LightBind:
        public NodeBind
    {
        LightBind():
            NodeBind(std::static_pointer_cast<Node>(std::make_shared<Light>()))
        {}
        explicit LightBind(const std::shared_ptr<Light>& p):
            NodeBind(std::static_pointer_cast<Node>(p))
        {}
        explicit LightBind(NodeBind nb): NodeBind(nb) {}
        virtual ~LightBind() {}
        Light* self() {
            return (Light*)n.get();
        }
        void set_dist(float d) {self()->dist(d);}
        void set_diffuse(Color c) {self()->diffuse(c);}
        void set_specular(Color c) {self()->specular(c);}
        float dist() { return self()->dist(); }
        Color diffuse() {return self()->diffuse();}
        Color specular() {return self()->specular();}
    };
    
    struct ParticleBind:
        public NodeBind
    {
        //ParticleBind():
        //    NodeBind(std::static_pointer_cast<Node>(std::make_shared<Particle>()))
        //{}
        explicit ParticleBind(std::string fn):
            NodeBind(std::static_pointer_cast<Node>(
                std::make_shared<Particle>(
                    qor()->resource_path(fn),
                    qor()->resources()
                )
            ))
        {}
        explicit ParticleBind(const std::shared_ptr<Particle>& p):
            NodeBind(std::static_pointer_cast<Node>(p))
        {}
        explicit ParticleBind(NodeBind nb): NodeBind(nb) {}
        virtual ~ParticleBind() {}
        Particle* self() {
            return (Particle*)n.get();
        }
        MeshBind mesh() {
            return MeshBind(self()->mesh());
        }
    };

    struct TrackerBind:
        public NodeBind
    {
        TrackerBind():
            NodeBind(std::static_pointer_cast<Node>(std::make_shared<Tracker>()))
        {}
        explicit TrackerBind(const std::shared_ptr<Tracker>& p):
            NodeBind(std::static_pointer_cast<Node>(p))
        {}
        explicit TrackerBind(NodeBind nb): NodeBind(nb) {}
        virtual ~TrackerBind() {}
        Tracker* self() {
            return (Tracker*)n.get();
        }

        void stop() {
            self()->track();
        }
        void track(NodeBind nh) {
            self()->track(nh.n);
        }
    };

    struct CameraBind:
        public TrackerBind
    {
        CameraBind():
            TrackerBind(std::static_pointer_cast<Tracker>(std::make_shared<Camera>(
                qor()->resources(), qor()->window()
            )))
        {}
        explicit CameraBind(const std::shared_ptr<Node>& p):
            TrackerBind(std::static_pointer_cast<Tracker>(p))
        {}
        explicit CameraBind(const std::shared_ptr<Camera>& p):
            TrackerBind(std::static_pointer_cast<Tracker>(p))
        {}
        explicit CameraBind(NodeBind nb): TrackerBind(nb) {}
        virtual ~CameraBind() {}
        void set_fov(float f) { self()->fov(f); }
        float get_fov() const { return self()->fov(); }
        Camera* self() {
            return (Camera*)n.get();
        }
        const Camera* self() const {
            return (const Camera*)n.get();
        }
        //Camera* ortho(bool bottom_origin) { n->ortho(bottom_origin); }
        //Camera* perspective() { n->perspective(); }
    };

    struct SoundBind:
        public NodeBind
    {
        //SoundBind():
        //    NodeBind(std::static_pointer_cast<Node>(std::make_shared<Sound>()))
        //{}
        SoundBind(std::string fn):
            NodeBind(std::static_pointer_cast<Node>(std::make_shared<Sound>(
                qor()->resource_path(fn),
                qor()->resources()
            )))
            //NodeBind(std::static_pointer_cast<Node>(std::make_shared<Sound>(
            //    fn,
            //    qor()->resources()
            //)))
        {}
        explicit SoundBind(const std::shared_ptr<Sound>& p):
            NodeBind(std::static_pointer_cast<Node>(p))
        {}
        explicit SoundBind(NodeBind nb): NodeBind(nb) {}
        virtual ~SoundBind() {}
        Sound* self() { return (Sound*)kit::safe_ptr(n.get()); }
        void play() { self()->play(); }
        void loop(bool b) { self()->loop(b); }
        void stop() { self()->stop(); }
        void pause() { self()->pause(); }
        bool playing() { return self()->playing(); }
        void ambient(bool b) { self()->ambient(b); }
        void detach_on_done() { self()->detach_on_done(); }
        void on_done(boost::python::object cb) {
            this->self()->on_done([cb]{
                cb();
            });
        }
    };

    struct SpriteBind:
        public NodeBind
    {
        //SpriteBind(NodeBind& nh) {
            // TODO: casting
        //}
        SpriteBind(std::string fn):
            NodeBind(std::static_pointer_cast<Node>(std::make_shared<Sprite>(
                qor()->resource_path(fn),
                qor()->resources()
            )))
        {
        }
        SpriteBind(std::string fn, std::string skin):
            NodeBind(std::static_pointer_cast<Node>(std::make_shared<Sprite>(
                qor()->resource_path(fn),
                qor()->resources(),
                skin
            )))
        {}
        SpriteBind(std::string fn, std::string skin, glm::vec3 pos):
            NodeBind(std::static_pointer_cast<Node>(std::make_shared<Sprite>(
                qor()->resource_path(fn),
                qor()->resources(),
                skin,
                pos
            )))
        {}
        SpriteBind(const std::shared_ptr<Sprite>& mesh):
            NodeBind(std::static_pointer_cast<Node>(mesh))
        {}
        explicit SpriteBind(NodeBind nb): NodeBind(nb) {}
        virtual ~SpriteBind() {}
        Sprite* self() {
            return (Sprite*)n.get();
        }
        unsigned state_id(const std::string& s) { return self()->state_id(s); }
        void state(unsigned id) { self()->set_state(id); }
        //void states(boost::python::list& l) {
        //    std::vector<unsigned> v;
        //    auto length = len(l);
        //    for(unsigned i=0;i<length;++i)
        //        v.push_back(i); // uhh....
        //    self()->set_states_by_id(v);
        //}
    };

    struct NodeInterfaceBind
    {
        std::shared_ptr<NodeInterface> n;
        NodeInterfaceBind() {}
        NodeInterfaceBind(const std::shared_ptr<NodeInterface>& p):
            n(p)
        {}
        virtual ~NodeInterfaceBind() {}
    };

    //struct Player2DBind:
    //    public NodeInterfaceBind
    //{
    //    //Player2DBind():
    //    //    NodeInterfaceBind(std::static_pointer_cast<NodeInterface>(
    //    //        std::make_shared<Player2DInterface>(
                    
    //    //        )
    //    //    ))
    //    //{}
    //    Player2DBind* self() {
    //        return (Player2DBind*)n.get();
    //    }
    //};

    struct Player3DBind:
        public NodeInterfaceBind
    {
        Player3DBind(){}
        Player3DBind(NodeBind nih):
            NodeInterfaceBind(std::static_pointer_cast<NodeInterface>(
                std::make_shared<PlayerInterface3D>(
                    qor()->session()->profile(0)->controller(),
                    nih.n,
                    nih.n
                )
            ))
        {
            ((PlayerInterface3D*)n.get())->plug();
        }
        virtual ~Player3DBind() {}
        PlayerInterface3D* self() {
            return (PlayerInterface3D*)n.get();
        }
        const PlayerInterface3D* self() const {
            return (const PlayerInterface3D*)n.get();
        }
        float get_speed() const {
            return self()->speed();
        }
        void set_speed(float s) {
            self()->speed(s);
        }

    };

    //NodeBind create(std::string type) {
    //    return NodeBind(qor()->make(""));
    //}

    //NodeBind spawn(std::string type) {
    //    auto n = qor()->nodes().create(type);
    //    qor()->current_state()->root()->add(n);
    //    return NodeBind(n);
    //}

    void render_from(NodeBind nh) {
        qor()->current_state()->camera(nh.n);
    }

    CameraBind camera() {
        return CameraBind(std::static_pointer_cast<Node>(
            qor()->current_state()->camera()
        ));
    }

    NodeBind root() {
        assert(qor()->current_state());
        assert(qor()->current_state()->root());
        return NodeBind(qor()->current_state()->root());
        //object main = py::import("__main__");
        //Context c = boost::python::extract<Context>(main.attr("context"));
    }
    boost::python::object find(std::string s) {
        return root().find(s);
    }

    void relative_mouse(bool b) {qor()->input()->relative_mouse(b);}
    void push_state(unsigned state) { qor()->push_state(state);}
    void pop_state() { qor()->pop_state(); }
    void change_state(std::string state) { qor()->change_state(state);}
    void quit() { qor()->quit(); }
    void perspective() {
        ((Camera*)qor()->current_state()->camera().get())->perspective();
    }
    void ortho(bool origin_bottom = false) {
        ((Camera*)qor()->current_state()->camera().get())->ortho(origin_bottom);
    }
    //object bg_color() {
    //}
    void bg_color(Color c) {
        qor()->pipeline()->bg_color(c);
    }
    unsigned screen_w() {
        return qor()->window()->size().x;
    }
    unsigned screen_h() {
        return qor()->window()->size().y;
    }
    void cache(std::string fn) {
        qor()->resources()->cache(fn);
    }
    void optimize() {
        qor()->resources()->optimize();
    }
    void script_error(const std::exception& e)
    {
        WARNING(e.what());
        PyErr_SetString(PyExc_RuntimeError, e.what());
    }
    void on_enter(boost::python::object cb) {
        qor()->current_state()->on_enter.connect([cb]{ cb(); });
    }
    void on_tick(boost::python::object cb) {
        qor()->current_state()->on_tick.connect([cb](Freq::Time t){ cb(t.s()); });
    }
#ifndef QOR_NO_PHYSICS
    void on_generate(boost::python::object cb) {
        qor()->current_state()->physics()->on_generate([cb]{
            cb();
        });
    }
#endif
    void log(std::string s) { LOG(s); }
    MetaBind state_meta() { return MetaBind(qor()->current_state()->meta()); }
    MetaBind meta() { return MetaBind(qor()->meta()); }

    MeshBind quad(float scale, std::string fn) {
        return MeshBind(std::make_shared<Mesh>(
            std::make_shared<MeshGeometry>(Prefab::quad()),
            std::vector<std::shared_ptr<IMeshModifier>>{
                std::make_shared<Wrap>(Prefab::quad_wrap()),
                std::make_shared<MeshNormals>(Prefab::quad_normals())
            },
            std::make_shared<MeshMaterial>(
                qor()->resources()->cache_cast<ITexture>(fn)
            )
        ));
    }
    MeshBind cube(float scale, std::string fn) {
        return MeshBind(std::make_shared<Mesh>(
            std::make_shared<MeshGeometry>(Prefab::cube()),
            std::vector<std::shared_ptr<IMeshModifier>>{
                std::make_shared<Wrap>(Prefab::cube_wrap()),
                std::make_shared<MeshNormals>(Prefab::cube_normals())
            },
            std::make_shared<MeshMaterial>(
                qor()->resources()->cache_cast<ITexture>(fn)
            )
        ));
    }
    
    float vec3_length(glm::vec3& v) { return v.length(); }
    void vec3_normalize(glm::vec3& v) { glm::normalize(v); }

    float get_x(glm::vec3& v) { return v.x; }
    float get_y(glm::vec3& v) { return v.y; }
    float get_z(glm::vec3& v) { return v.z; }
    void set_x(glm::vec3& v, float x) { v.x = x; }
    void set_y(glm::vec3& v, float y) { v.y = y; }
    void set_z(glm::vec3& v, float z) { v.z = z; }

    //float get_r(Color c) { return c.r; }
    //float get_g(Color c) { return c.g; }
    //float get_b(Color c) { return c.b; }
    //float get_a(Color c) { return c.a; }
    //void set_r(Color c, float r) { c.r = r; }
    //void set_g(Color c, float g) { c.g = g; }
    //void set_b(Color c, float b) { c.b = b; }
    //void set_a(Color c, float b) { c.a = a; }

    //void restart_state() { qor()->restart_state(); }

    void on_event(std::string ev, boost::python::object cb) {
        qor()->current_state()->event(ev, [cb](std::shared_ptr<Meta> m){
            cb(MetaBind(m));
        });
    }
    void event(std::string ev, MetaBind m) {
        qor()->current_state()->event(ev, m.m);
    }
    bool has_event(std::string ev) {
        return qor()->current_state()->has_event(ev);
    }
    bool exists(std::string fn){
        return qor()->exists(fn);
    }

    boost::python::list get_collisions_for(NodeBind a) {
        auto v = qor()->pipeline()->partitioner()->get_collisions_for(a.n.get());
        boost::python::list l;
        for(Node* n: v)
            l.append<NodeBind>(NodeBind(n));
        return l;
    }
    boost::python::list get_collisions_for_t(NodeBind a, unsigned b) {
        auto v = qor()->pipeline()->partitioner()->get_collisions_for(a.n.get(), b);
        boost::python::list l;
        for(Node* n: v)
            l.append<NodeBind>(NodeBind(n));
        return l;
    }

    void on_collision(NodeBind a, NodeBind b, boost::python::object cb){
        qor()->pipeline()->partitioner()->on_collision(a.n, b.n, [cb](Node* aa, Node* bb){
            cb(NodeBind(aa), NodeBind(bb));
        });
    }
    void on_no_collision(NodeBind a, NodeBind b, boost::python::object cb){
        qor()->pipeline()->partitioner()->on_collision(a.n, b.n,
            std::function<void(Node*,Node*)>(),
            [cb](Node* aa, Node* bb){
                cb(NodeBind(aa), NodeBind(bb));
            }
        );
    }
    void on_touch(NodeBind a, NodeBind b, boost::python::object cb){
        qor()->pipeline()->partitioner()->on_collision(a.n, b.n,
            std::function<void(Node*,Node*)>(),
            std::function<void(Node*,Node*)>(),
            [cb](Node* aa, Node* bb){
                cb(NodeBind(aa), NodeBind(bb));
            }
        );
    }
    void on_untouch(NodeBind a, NodeBind b, boost::python::object cb){
        qor()->pipeline()->partitioner()->on_collision(a.n, b.n,
            std::function<void(Node*,Node*)>(),
            std::function<void(Node*,Node*)>(),
            std::function<void(Node*,Node*)>(),
            [cb](Node* aa, Node* bb){
                cb(NodeBind(aa), NodeBind(bb));
            }
        );
    }

    void on_collision_t(NodeBind a, unsigned b, boost::python::object cb)
    {
        qor()->pipeline()->partitioner()->on_collision(a.n, b, [cb](Node* aa, Node* bb){
            cb(NodeBind(aa), NodeBind(bb));
        });
    }
    void on_no_collision_t(NodeBind a, unsigned b, boost::python::object cb)
    {
        qor()->pipeline()->partitioner()->on_collision(a.n, b,
            std::function<void(Node*,Node*)>(),
            [cb](Node* aa, Node* bb){
                cb(NodeBind(aa), NodeBind(bb));
            }
        );
    }
    void on_touch_t(NodeBind a, unsigned b, boost::python::object cb)
    {
        qor()->pipeline()->partitioner()->on_collision(a.n, b,
            std::function<void(Node*,Node*)>(),
            std::function<void(Node*,Node*)>(),
            [cb](Node* aa, Node* bb){
                cb(NodeBind(aa), NodeBind(bb));
            }
        );
    }
    void on_untouch_t(NodeBind a, unsigned b, boost::python::object cb)
    {
        qor()->pipeline()->partitioner()->on_collision(a.n, b,
            std::function<void(Node*,Node*)>(),
            std::function<void(Node*,Node*)>(),
            std::function<void(Node*,Node*)>(),
            [cb](Node* aa, Node* bb){
                cb(NodeBind(aa), NodeBind(bb));
            }
        );
    }

    void on_collision_tt(unsigned a, unsigned b, boost::python::object cb)
    {
        qor()->pipeline()->partitioner()->on_collision(a, b, [cb](Node* aa, Node* bb){
            cb(NodeBind(aa), NodeBind(bb));
        });
    }
    void on_no_collision_tt(unsigned a, unsigned b, boost::python::object cb)
    {
        qor()->pipeline()->partitioner()->on_collision(a, b,
            std::function<void(Node*,Node*)>(),
            [cb](Node* aa, Node* bb){
                cb(NodeBind(aa), NodeBind(bb));
            }
        );
    }
    void on_touch_tt(unsigned a, unsigned b, boost::python::object cb)
    {
        qor()->pipeline()->partitioner()->on_collision(a, b,
            std::function<void(Node*,Node*)>(),
            std::function<void(Node*,Node*)>(),
            [cb](Node* aa, Node* bb){
                cb(NodeBind(aa), NodeBind(bb));
            }
        );
    }
    void on_untouch_tt(unsigned a, unsigned b, boost::python::object cb)
    {
        qor()->pipeline()->partitioner()->on_collision(a, b,
            std::function<void(Node*,Node*)>(),
            std::function<void(Node*,Node*)>(),
            std::function<void(Node*,Node*)>(),
            [cb](Node* aa, Node* bb){
                cb(NodeBind(aa), NodeBind(bb));
            }
        );
    }

    void clear_collisions() {
        qor()->pipeline()->partitioner()->clear();
    }
    void register_object(NodeBind n, unsigned type) {
        qor()->pipeline()->partitioner()->register_object(n.n,type);
    }

    glm::vec3 box_min(Box& b) { return b.min(); }
    glm::vec3 box_max(Box& b) { return b.max(); }
    void box_set_min(Box& b, glm::vec3 v) { b.min() = v; }
    void box_set_max(Box& b, glm::vec3 v) { b.max() = v; }

    void uniform(std::string name, float v)
    {
        if(Headless::enabled())
            return;
        for(int i=0; i<2; ++i){
            qor()->pipeline()->shader(i)->use();
            int u = qor()->pipeline()->shader(i)->uniform(name);
            if(u >= 0)
                qor()->pipeline()->shader(i)->uniform(u,v);
        }
    }

    bool is_server(){
        return Headless::server();
    }

#ifndef QOR_NO_PHYSICS
    void set_gravity(glm::vec3 v) {
        qor()->current_state()->physics()->gravity(v);
    }
    glm::vec3 gravity() {
        return qor()->current_state()->physics()->gravity();
    }
    void on_physics_collision(NodeBind a, boost::python::object cb){
        qor()->current_state()->physics()->on_collision(a.n.get(),[cb](
            Node* aa, Node* bb, glm::vec3 ap, glm::vec3 bp, glm::vec3 bn
        ){
            cb(NodeBind(aa->as_node()), NodeBind(bb->as_node()), ap, bp, bn);
        });
    }
    
#endif

    BOOST_PYTHON_MODULE(qor)
    {
        boost::python::register_exception_translator<std::exception>(&script_error);
        
        //def("spawn", spawn, boost::python::args("name"));
        boost::python::def("root", root);
        boost::python::def("camera", camera);
        boost::python::def("relative_mouse", relative_mouse);
        boost::python::def("push_state", push_state, boost::python::args("state"));
        boost::python::def("pop_state", pop_state);
        boost::python::def("change_state", change_state, boost::python::args("state"));
        boost::python::def("quit", quit);
        boost::python::def("ortho", ortho, boost::python::args("origin_bottom"));
        boost::python::def("perspective", perspective);
        boost::python::def("bg_color", bg_color, boost::python::args("rgb"));
        boost::python::def("render_from", render_from, boost::python::args("camera"));
        boost::python::def("screen_w", screen_w);
        boost::python::def("screen_h", screen_h);
        boost::python::def("cache", cache, boost::python::args("fn"));
        boost::python::def("optimize", optimize);
        boost::python::def("find", find);
        boost::python::def("on_enter", on_enter);
        boost::python::def("on_tick", on_tick);
        boost::python::def("exists", &Qor::exists);
        boost::python::def("log", log);
        boost::python::def("meta", meta);
        //def("session_meta", session_meta);
        boost::python::def("state_meta", state_meta);
        boost::python::def("quad", quad);
        boost::python::def("cube", cube);
        boost::python::def("uniform", uniform);
        boost::python::def("headless", Headless::enabled);
        boost::python::def("server", is_server);

#ifndef QOR_NO_PHYSICS
        boost::python::def("gravity", gravity);
        boost::python::def("gravity", set_gravity);
        boost::python::def("on_physics_collision", on_physics_collision);
#endif
        
        boost::python::def("on_event", on_event);
        boost::python::def("event", event);
        boost::python::def("has_event", has_event);

        boost::python::def("get_collisions_for", get_collisions_for);
        boost::python::def("get_collisions_for", get_collisions_for_t);
        
        boost::python::def("on_collision", on_collision);
        boost::python::def("on_collision", on_collision_t);
        boost::python::def("on_collision", on_collision_tt);
        
        boost::python::def("on_no_collision", on_no_collision);
        boost::python::def("on_no_collision", on_no_collision_t);
        boost::python::def("on_no_collision", on_no_collision_tt);
        
        boost::python::def("on_touch", on_touch);
        boost::python::def("on_touch", on_touch_t);
        boost::python::def("on_touch", on_touch_tt);
        
        boost::python::def("on_untouch", on_untouch);
        boost::python::def("on_untouch", on_untouch_t);
        boost::python::def("on_untouch", on_untouch_tt);
        
        boost::python::def("clear_collisions", clear_collisions);
        boost::python::def("register_object", register_object);

        //def("to_string", Vector::to_string);
        //def("to_string", Matrix::to_string);

        boost::python::enum_<Space>("Space")
            .value("LOCAL", Space::LOCAL)
            .value("PARENT", Space::PARENT)
            .value("WORLD", Space::WORLD)
        ;
        boost::python::enum_<Node::Physics>("PhysicsType")
            .value("NO_PHYSICS", Node::Physics::NO_PHYSICS)
            .value("STATIC", Node::Physics::STATIC)
            .value("DYNAMIC", Node::Physics::DYNAMIC)
            .value("ACTOR", Node::Physics::ACTOR)
            .value("GHOST", Node::Physics::GHOST)
            .value("KINEMATIC", Node::Physics::KINEMATIC)
        ;
        boost::python::enum_<Node::PhysicsShape>("PhysicsShape")
            .value("NO_SHAPE", Node::PhysicsShape::NO_SHAPE)
            .value("MESH", Node::PhysicsShape::MESH)
            .value("HULL", Node::PhysicsShape::HULL)
            .value("BOX", Node::PhysicsShape::BOX)
            .value("SPHERE", Node::PhysicsShape::SPHERE)
            .value("CAPSULE", Node::PhysicsShape::CAPSULE)
            .value("CYLINDER", Node::PhysicsShape::CYLINDER)
        ;

        //boost::python::enum_<eNode>("NodeType")
        //    .value("NODE", eNode::NODE)
        //    .value("SPRITE", eNode::SPRITE)
        //    .value("ENVIRONMENT", eNode::ENVIRONMENT)
        //    .value("ENTITY", eNode::ENTITY)
        //    .value("ACTOR", eNode::ACTOR)
        //    .value("LIGHT", eNode::LIGHT)
        //    .value("PARTICLE_SYSTEM", eNode::PARTICLE_SYSTEM)
        //    .value("SOUND", eNode::SOUND)
        //    .value("SELECTION", eNode::SELECTION)
        //    .value("TRACKER", eNode::TRACKER)
        //    .value("CAMERA", eNode::CAMERA)
        //    //.value("USER_TYPE", eNode::USER_TYPE)
        //    //.value("MAX", eNode::MAX),
        //;

        //boost::python::class_<Window>("Window")
        //    .add_property("position", &WindowBind::get_position, &WindowBind::set_position)
        //    .add_property("center", &WindowBind::get_position, &WindowBind::set_position)
        
        //boost::python::class_<ContextBind>("Context", no_init);
        boost::python::class_<MetaBind>("MetaBind")
            //.def(boost::python::init<>())
            .def("empty", &MetaBind::empty)
        ;
        
        boost::python::class_<glm::vec3>("vec3")
            .def(boost::python::init<>())
            .def(boost::python::init<float>())
            .def(boost::python::init<float,float,float>())
            .def(boost::python::self + boost::python::self)
            .def(boost::python::self - boost::python::self)
            .def(boost::python::self * boost::python::self)
            .def(boost::python::self * float())
            .def(boost::python::self += boost::python::self)
            .def(boost::python::self -= boost::python::self)
            .def(boost::python::self *= boost::python::self)
            .def(boost::python::self *= float())
            .add_property("x", &get_x, &set_x)
            .add_property("y", &get_y, &set_y)
            .add_property("z", &get_z, &set_z)
            .def("length", &vec3_length)
            .def("normalize", &vec3_normalize)
        ;
        boost::python::class_<glm::vec4>("vec4")
            .def(boost::python::init<>())
            .def(boost::python::init<float>())
            .def(boost::python::init<float,float,float,float>())
            .def(boost::python::self + boost::python::self)
            .def(boost::python::self - boost::python::self)
            .def(boost::python::self * boost::python::self)
            .def(boost::python::self * float())
            .def(boost::python::self += boost::python::self)
            .def(boost::python::self -= boost::python::self)
            .def(boost::python::self *= boost::python::self)
            .def(boost::python::self *= float())
            //.def("length", &glm::length<float>)
            //.def("normalize", &glm::normalize<float>)
        ;

        boost::python::class_<Color>("Color")
            .def(boost::python::init<>())
            .def(boost::python::init<float>())
            .def(boost::python::init<std::string>())
            .def(boost::python::init<float,float,float>())
            .def(boost::python::init<float,float,float,float>())
            //.def("r", &Color::r)
            //.def("g", &Color::g)
            //.def("b", &Color::b)
            //.def("a", &Color::a)
            .def(boost::python::self + boost::python::self)
            .def(boost::python::self - boost::python::self)
            .def(boost::python::self * boost::python::self)
            .def(boost::python::self * float())
            .def(boost::python::self += boost::python::self)
            .def(boost::python::self -= boost::python::self)
            .def(boost::python::self *= boost::python::self)
            .def(boost::python::self *= float())
            //.def("saturate", &Color::saturate)
            .def("glm::vec3", &Color::vec3)
            .def("glm::vec4", &Color::vec4)
            .def("string", &Color::string)
        ;
        
        boost::python::class_<Box>("Box")
            //.def(boost::python::init<>())
            .def(boost::python::init<glm::vec3,glm::vec3>())
            .def("min", &box_min)
            .def("max", &box_max)
            .def("min", &box_set_min)
            .def("max", &box_set_max)
            .def("size", &Box::size)
            .def("center", &Box::center)
        ;

        //boost::python::class_<StateMachine>("StateMachine")
        //    .def(boost::python::init<>())
        //    .def("on_tick", &statemachine_on_tick)
        //    //.def("__call__", &StateMachine::operator())
        //;
        
        boost::python::class_<NodeBind>("Node")
            //.def(boost::python::init<>())
            .def("discard", &NodeBind::discard)
            .def("nullify", &NodeBind::nullify)
            .def("visible", &NodeBind::set_visible)
            .def("visible", &NodeBind::get_visible)
            .def("self_visible", &NodeBind::set_self_visible)
            .def("self_visible", &NodeBind::get_self_visible)
            .add_property("matrix", &NodeBind::get_matrix, &NodeBind::set_matrix)
            //.def("position", &NodeBind::position)
            .def("position", &NodeBind::set_position, node_set_position_overloads())
            .def("position", &NodeBind::get_position, node_get_position_overloads())
            .def("velocity", &NodeBind::get_velocity)
            .def("velocity", &NodeBind::set_velocity)
            .def("rotate", &NodeBind::rotate)
            .def("move", &NodeBind::move)
            .def("scale", &NodeBind::scale)
            //.def("rescale", &NodeBind::rescale)
            .def("__nonzero__", &NodeBind::valid)
            .def("pend", &NodeBind::pend)
            .def("num_descendents", &NodeBind::num_descendents)
            .def("num_children", &NodeBind::num_children)
            .def("children", &NodeBind::children)
            .def("add", &NodeBind::add)
            .def("stick", &NodeBind::stick)
            .def("parent", &NodeBind::parent)
            .def("root", &NodeBind::root)
            .def("subroot", &NodeBind::subroot)
            .def("spawn", &NodeBind::spawn)
            .def("as_node", &NodeBind::as_node)
            .def("detach", &NodeBind::detach)
            .def("collapse", &NodeBind::collapse, boost::python::args("space"))
            .def("add_tag", &NodeBind::add_tag)
            .def("has_tag", &NodeBind::has_tag)
            .def("remove_tag", &NodeBind::remove_tag)
            .def("find", &NodeBind::find)
            .def("find_if", &NodeBind::find_if)
            .def("find_type", &NodeBind::find_type)
            .def("on_tick", &NodeBind::on_tick)
#ifndef QOR_NO_PHYSICS
            .def("generate", &NodeBind::generate)
#endif
            .def("event", &NodeBind::event)
            .def("on_event", &NodeBind::on_event)
            .def("has_event", &NodeBind::has_event)
            
            .def("state", &NodeBind::state)
            .def("state", &NodeBind::set_state)
            .def("on_tick", &NodeBind::on_state_tick)
            .def("on_enter", &NodeBind::on_state_enter)
            .def("on_leave", &NodeBind::on_state_leave)
            .def("on_attempt", &NodeBind::on_state_attempt)
            .def("on_reject", &NodeBind::on_state_reject)
            .def("clear_state", &NodeBind::clear_state)
            .def("clear_states", &NodeBind::clear_states)
            
            .def("physics", &NodeBind::physics)
            .def("physics_shape", &NodeBind::physics_shape)
            //.def_readonly("type", &NodeBind::type)
            //.def("add", &NodeBind::add)
            .def("each", &NodeBind::each)

            .def("box", &NodeBind::box)
            .def("world_box", &NodeBind::world_box)
        ;
        boost::python::class_<MeshBind, boost::python::bases<NodeBind>>("Mesh")
            //.def(boost::python::init<>())
            .def(boost::python::init<std::string>())
            .def(boost::python::init<NodeBind>())
            .def("instance", &MeshBind::instance)
            .def("prototype", &MeshBind::prototype)
            .def("set_physics_shape", &MeshBind::set_physics_shape)
            .def("set_physics", &MeshBind::set_physics)
            .def("mass", &MeshBind::get_mass)
            .def("mass", &MeshBind::set_mass)
            .def("friction", &MeshBind::friction)
            .def("impulse", &MeshBind::impulse)
            .def("gravity", &MeshBind::gravity)
            .def("inertia", &MeshBind::inertia)
            .def("material", &MeshBind::material)
            .def("swap_material", &MeshBind::swap_material)
            .def("clear_body", &MeshBind::clear_body)
            .def("teleport", &MeshBind::teleport)
        ;
        boost::python::class_<SpriteBind, boost::python::bases<NodeBind>>("Sprite", boost::python::init<std::string>())
            .def(boost::python::init<NodeBind>())
            .def(boost::python::init<std::string>())
            .def(boost::python::init<std::string, std::string>())
            .def(boost::python::init<std::string, std::string, glm::vec3>())
            .def("state", &SpriteBind::state)
            //.def("states", &SpriteBind::states)
            .def("state_id", &SpriteBind::state_id)
        ;
        boost::python::class_<TrackerBind, boost::python::bases<NodeBind>>("Tracker", boost::python::init<>())
            .def(boost::python::init<NodeBind>())
            .def("stop", &TrackerBind::stop)
            .def("track", &TrackerBind::track, boost::python::args("node"))
        ;
        boost::python::class_<CameraBind, boost::python::bases<TrackerBind>>("Camera", boost::python::init<>())
            .def(boost::python::init<NodeBind>())
            .add_property("fov", &CameraBind::get_fov, &CameraBind::set_fov)
            //.def("ortho", &Camera::ortho, boost::python::args("origin_bottom"))
            //.def("perspective", &Camera::perspective)
        ;
        boost::python::class_<LightBind, boost::python::bases<NodeBind>>("Light", boost::python::init<>())
            .def(boost::python::init<NodeBind>())
            .def("diffuse", &LightBind::diffuse)
            .def("diffuse", &LightBind::set_diffuse)
            .def("specular", &LightBind::specular)
            .def("specular", &LightBind::set_specular)
            .def("dist", &LightBind::dist)
            .def("dist", &LightBind::set_dist)
        ;
        boost::python::class_<ParticleBind, boost::python::bases<NodeBind>>("Particle", boost::python::init<std::string>())
            .def(boost::python::init<NodeBind>())
            .def("mesh", &ParticleBind::mesh)
        ;
        boost::python::class_<SoundBind, boost::python::bases<NodeBind>>("Sound", boost::python::init<std::string>())
            .def(boost::python::init<NodeBind>())
            .def(boost::python::init<std::string>())
            .def("play", &SoundBind::play)
            .def("pause", &SoundBind::pause)
            .def("stop", &SoundBind::stop)
            .def("playing", &SoundBind::playing)
            .def("ambient", &SoundBind::ambient)
            .def("detach_on_done", &SoundBind::detach_on_done)
            .def("on_done", &SoundBind::on_done)
            .def("loop", &SoundBind::loop)
        ;
        boost::python::class_<NodeInterfaceBind>("NodeInterface")
        ;
        //boost::python::class_<Player2DBind, boost::python::bases<NodeInterfaceBind>>("Player2D", boost::python::init<>())
        //;
        boost::python::class_<Player3DBind, boost::python::bases<NodeInterfaceBind>>("Player3D", boost::python::init<NodeBind>())
            .add_property("speed", &Player3DBind::get_speed, &Player3DBind::set_speed)
        ;
    }

}

#endif
#endif

