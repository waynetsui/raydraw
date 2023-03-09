#include <iostream>
#include "raylib.h"
#include "rlgl.h"
#include <vector>
#include <memory>

#define MAX_CONTACT_POINTS 64
#define MAX_CONTACT_POINTS_MAK 63

using namespace std;
// using Path = vector<Vector2>;

bool Vector2Equal(Vector2 &a, Vector2 &b)
{
    return (a.x == b.x && a.y == b.y);
}

namespace contact {
    enum class ContactState
    {
        None,
        Begin,
        Move,
        End
    };

    enum class ContactType {
        None,
        Pen,
        Eraser,
        Finger,
        Mouse,
    };

    struct Contact {
        ContactState state;
        int id;
        ContactType type;
        Vector2 position;
        float pressure;
        Contact() :state(ContactState::None), id(-1), type(ContactType::None), position(Vector2{0,0}), pressure(0) {}
        Contact(ContactState _state,
        int _id,
        ContactType _type,
        Vector2 _position,
        float _pressure):
            state(_state),
            id(_id),
            type(_type),
            position(_position),
            pressure(_pressure)
        {
        }
    };
}

using namespace contact;

static struct  Contact touchs[MAX_CONTACT_POINTS]{};
static struct Contact mouse {};

inline bool valid_contact_id(int id) {
    return id <= MAX_CONTACT_POINTS && id >= 0;
}

//
//inline bool contact_id_exsit(int id) {
//     return valid_id(id) && contacts[id].state != ContactState::None;
//}

//bool contact_path_push(int id, const Vector2& position, float pressure)
//{
//	if (!valid_id(id))
//		return false;
//    if (contacts[id].state == ContactState::None) {
//        // create path and connect
//    }else
//	contacts[id].state = ContactState::Begin;
//	contacts[id].position = position;
//	contacts[id].pressure = pressure;
//	return true;
//}

// 0 for mouse, others for touch
static ContactState contact_state[MAX_CONTACT_POINTS+1] = {ContactState::None};

inline int get_contact_id(int touch_id)
{
    if (touch_id < 0 || touch_id >= MAX_CONTACT_POINTS)
        return -1;
    return touch_id + 1;
}

size_t getContacts(vector<Contact> &contactsv)
{
    // get touch
    bool stage[MAX_CONTACT_POINTS + 1] = { false };
    size_t count = GetTouchPointCount();
    for (int i = 0; i < count; i++)
    {
        Contact contact{};
        contact.id = get_contact_id(GetTouchPointId(i));
        if (!valid_contact_id(contact.id))
            continue;
        if (contact_state[contact.id] == ContactState::None)
            contact.state = ContactState::Begin;
        else
            contact.state = ContactState::Move;
        stage[contact.id] = true;
        contact_state[contact.id] = contact.state;
        contact.position = GetTouchPosition(i);
        contact.type = ContactType::Finger;
        contactsv.push_back(contact);
    }
    for (int i = 1; i <= MAX_CONTACT_POINTS; i++) {
        if (!stage[i] && contact_state[i] != ContactState::None) {
            if (contact_state[i] != ContactState::End) {
                Contact contact{};
                contact.id = i;
                contact.state = ContactState::End;
                contact.type = ContactType::Finger;
                contact_state[i] = contact.state;
                contactsv.push_back(contact);
            }
            else {
                contact_state[i] = ContactState::None;
            }
        }
    }


    // get mouse
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        mouse.type = ContactType::Mouse;
        mouse.id = 0;
        mouse.position = GetMousePosition();
        if (mouse.state == ContactState::None)
            mouse.state = ContactState::Begin;
        else
            mouse.state = ContactState::Move;
        contactsv.push_back(mouse);
    }
    else if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
        if (mouse.state != ContactState::None) {
            if (mouse.state == ContactState::End) {
                mouse.state = ContactState::None;
            }
            else {
                mouse.state = ContactState::End;
                contactsv.push_back(mouse);
            }
        }
    }
    return count;
}

struct Path {
    vector<Vector2> points;
    Color color;
    float width;
    Path() :color(ORANGE), width(2) {}
    Path(Color _color, float _width) :color(_color), width(_width) {}
};

using PathPtr = shared_ptr<Path>;

static vector<PathPtr> paths;
PathPtr tracking_path[MAX_CONTACT_POINTS + 1]={nullptr};

void create_path(Contact &contact)
{
    PathPtr path = make_shared<Path>();
    path->points.push_back(contact.position);
    tracking_path[contact.id] = path;
    paths.push_back(path);
}

void updated_path(Contact &contact)
{
    if (tracking_path[contact.id] != nullptr) {
        if(!Vector2Equal(tracking_path[contact.id]->points.back(), contact.position))
            tracking_path[contact.id]->points.push_back(contact.position);
    }
}

void draw_paths()
{
    for (auto& path : paths)
    {
        if (path->points.size() > 1)
            DrawLineStrip(path->points.data(), path->points.size(), path->color);
    }
}

void draw_contacts(vector<Contact> &contacts)
{
    for (auto& contact : contacts)
    {
        if (contact.state == ContactState::Begin) {
            create_path(contact);
        }
        else if(contact.state == ContactState::Move) {
            updated_path(contact);
        }
	}

    draw_paths();
}

int main()
{
    bool path_valid = false;
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    Path *currentPath = nullptr;

    SetConfigFlags(FLAG_MSAA_4X_HINT); //4次多重采样，以达到抗锯齿的效果
    InitWindow(screenWidth, screenHeight, "mouse input");
    ToggleFullscreen();
    SetTargetFPS(60);

    int index = 0;
    rlSetLineWidth(2);
    //rlEnableSmoothLines();
    vector<Contact> contactsv(MAX_CONTACT_POINTS);
    while (!WindowShouldClose())
    {
        contactsv.clear();
        getContacts(contactsv);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw_contacts(contactsv);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}