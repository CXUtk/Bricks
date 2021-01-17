#include "DelaunayScene.h"
#include "Game.h"
#include "DCEL.h"

#include <queue>
#include <set>
#include <ctime>

Vertex vPool[MAXN];
int totV;

Face fPool[MAXN * 2];
int totF;

Edge ePool[MAXN * 6];
int totE;



typedef struct vec3_t {
    double x, y, z;
    vec3_t(double _x = 0, double _y = 0, double _z = 0) {
        x = _x, y = _y, z = _z;
    }
    inline friend vec3_t operator+(const vec3_t& a, const vec3_t& b) {
        return vec3_t(a.x + b.x, a.y + b.y, a.z + b.z);
    }
    inline friend vec3_t operator-(const vec3_t& a, const vec3_t& b) {
        return vec3_t(a.x - b.x, a.y - b.y, a.z - b.z);
    }
    inline friend vec3_t operator*(const vec3_t& a, double k) {
        return vec3_t(a.x * k, a.y * k, a.z * k);
    }
    inline friend vec3_t cross(const vec3_t& a, const vec3_t& b) {
        return vec3_t(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x);
    }
    inline friend double dot(const vec3_t& a, const vec3_t& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
} node3_t;

bool inCircumcircle(Vertex* a, Vertex* b, Vertex* c, Vertex* d) {
    //vec3_t A = vec3_t(a->pos.x, a->pos.y, Vector2::dot(a->pos, a->pos));
    //vec3_t B = vec3_t(b->pos.x, b->pos.y, Vector2::dot(b->pos, b->pos));
    //vec3_t C = vec3_t(c->pos.x, c->pos.y, Vector2::dot(c->pos, c->pos));
    //vec3_t D = vec3_t(d->pos.x, d->pos.y, Vector2::dot(d->pos, d->pos));
    glm::dvec2 A = (a->pos - d->pos).to_dvec2();
    glm::dvec2 B = (b->pos - d->pos).to_dvec2();
    glm::dvec2 C = (c->pos - d->pos).to_dvec2();
    //if (Vector2::cross(b->pos - a->pos, c->pos - a->pos) < 0) {
    //    std::swap(B, C);
    //}
    if (Vector2::cross(b->pos - a->pos, c->pos - a->pos) < 0) {
        return false;
    }

    glm::dmat3 mat(glm::dvec3(A.x, B.x, C.x), glm::dvec3(A.y, B.y, C.y), glm::dvec3(glm::dot(A, A), glm::dot(B, B), glm::dot(C, C)));
    auto judge = glm::determinant(mat);
    return judge > 0;



    //node3_t normal = cross(B - A, C - A);
    //auto res = dot(normal, D - A);
    //if (res > EPS)
    //    return false;
    //else
    //    return true;
}


Vertex* newVertex(double x, double y) {
    ++totV;
    vPool[totV] = Vertex(totV, x, y);
    return &vPool[totV];
}

Edge* newEdge(Vertex* fr, Vertex* to) {
    ++totE;
    ePool[totE] = Edge(fr, to);
    ePool[totE].id = totE;
    return &ePool[totE];
}

// 创造一个以abc为逆时针顶点的三角形面
void constructNewFace(Face* face, Vertex* a, Vertex* b, Vertex* c) {
    Edge* edges[3];
    edges[0] = newEdge(a, b);
    edges[1] = newEdge(b, c);
    edges[2] = newEdge(c, a);

    for (int i = 0; i < 3; i++) {
        face->vs[i] = edges[i]->from->pos;
        edges[i]->next = edges[(i + 1) % 3];
        edges[(i + 1) % 3]->prev = edges[i];
        edges[i]->face = face;
    }
    face->edge = edges[0];
}


void constructFace(Face* face, Edge* a, Edge* b, Edge* c) {
    Edge* edges[3];
    edges[0] = a;
    edges[1] = b;
    edges[2] = c;
    for (int i = 0; i < 3; i++) {
        face->vs[i] = edges[i]->from->pos;
        edges[i]->next = edges[(i + 1) % 3];
        edges[(i + 1) % 3]->prev = edges[i];
        edges[i]->face = face;
    }
    face->edge = a;
}




Face* newFace() {
    totF++;
    fPool[totF].id = totF;
    return &fPool[totF];
}


DelaunayScene::DelaunayScene() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    auto& game = Game::GetInstance();
    game.getGraphics()->setProjectionMatrix(glm::ortho(0.f, 600.f, 0.f, 600.f, -1.0f, 1.0f));
    for (int i = 0; i < MAX_VERTICES; i++) {
        int r = (i / 25);
        int c = i % 25;
        vPool[++totV] = Vertex(i + 1, 30 + mt() % 500, 30 + mt() % 500);
        vPool[totV].shake();
    }

    //vPool[++totV] = Vertex(1, 30, 30);
    //vPool[totV].shake();
    //vPool[++totV] = Vertex(2, 31, 31);
    //vPool[totV].shake();
    //vPool[++totV] = Vertex(3, 32, 32);
    //vPool[totV].shake();

    auto a = newVertex(1e6, 1e6);
    auto b = newVertex(-1e6, 1e6);
    auto c = newVertex(-1e6, -1e6);
    auto d = newVertex(1e6, -1e6);


    auto f1 = newFace();
    auto f2 = newFace();
    constructNewFace(f1, a, b, d);
    constructNewFace(f2, d, b, c);

    f1->edge->next->twin = f2->edge;
    f2->edge->twin = f1->edge->next;

    for (int i = 1; i <= MAX_VERTICES; i++) {
        Vertex& v = vPool[i];
        v.testInTriangle(f1);
        v.testInTriangle(f2);
    }


    //printf("%d\n", Vector2::cross(vPool[2].pos - vPool[1].pos, vPool[3].pos - vPool[1].pos) >= 0);

    //clock_t startTime, endTime;
    //startTime = clock();
    // insert(394);
    //for (int i = 1; i <= MAX_VERTICES; i++) {
    //    insert(i);
    //}
    ////endTime = clock();  //计时结束
    ////printf("The running time is: %.4fs\n",
    ////    (double)(endTime - startTime) / CLOCKS_PER_SEC);
    //for (int i = 1; i <= totF; i++) {
    //    auto e = fPool[i].edge;
    //    for (int j = 0; j < 3; j++) {
    //        int u = e->from->id, v = e->to->id;
    //        printf("%d %d\n", u, v);
    //        e = e->next;
    //    }
    //    printf("\n");
    //}
}

DelaunayScene::~DelaunayScene() {
}

int t = 0;
int t2 = 0;
bool start = false;

void DelaunayScene::update() {
    auto input = Game::GetInstance().getInputManager();
    input->beginInput();
    if (input->getIsKeyDown('Z')) {
        start = true;
    }
    input->endInput();
    if (start) {
        if (t2 == 0) {
            if (t < MAX_VERTICES)
                insert(++t);

        }
        ++t2;
        if (t2 == 7) {
            t2 = 0;
        }
    }

}


bool vis[MAXN * 2];

void DelaunayScene::draw() {

    std::vector<Segment> segments;
    for (int i = 1; i <= totV; i++) {
        Vertex& v = vPool[i];
        Game::GetInstance().getGraphics()->drawCircle(glm::vec2(v.pos.x - 3, v.pos.y - 3), glm::vec2(6), glm::vec3(1, 0, 0));
    }

    if (t < MAX_VERTICES) {

        for (int i = 1; i <= totF; i++) {
            Face& f = fPool[i];
            auto& e = f.edge;
            for (int j = 0; j < 3; j++) {
                auto v = e->to->pos - e->from->pos;
                v = v.unit();
                Game::GetInstance().getGraphics()->drawDirectedArrow((e->from->pos).to_vec2(), (e->to->pos).to_vec2(), glm::vec3(1), 1, segments);
                e = e->next;
            }
        }


        Game::GetInstance().getGraphics()->drawLines(segments, glm::vec3(1), 1);
    }
    segments.clear();

    for (int i = 1; i <= totF; i++) {
        vis[i] = false;
    }

    std::queue<int> Q;
    Q.push(1);
    while (!Q.empty()) {
        auto f = Q.front();
        Q.pop();
        Face& face = fPool[f];

        if (vis[face.id]) continue;
        vis[face.id] = true;

        Edge* e = face.edge;
        Vector2 center = face.getCircumcentre();
        for (int i = 0; i < 3; i++) {
            auto twin = e->twin;
            if (twin) {
                Face* nextFace = twin->face;
                if (!vis[nextFace->id]) {
                    Vector2 c2 = nextFace->getCircumcentre();
                    segments.push_back(Segment(center.to_vec2(), c2.to_vec2()));
                    Q.push(nextFace->id);
                }
            }
            e = e->next;
        }
    }

    Game::GetInstance().getGraphics()->drawLines(segments, glm::vec3(1, 1, 0), 1);
}

void DelaunayScene::insert(int x) {
    Vertex* v = &vPool[x];
    Face* face = v->belong;

    // split orignal face into 3 smaller faces

    auto fEdge = face->edge;
    Face* nFaces[3];
    nFaces[0] = face;
    nFaces[1] = newFace();
    nFaces[2] = newFace();

    // nEdges 是逆时针的第一条边，cEdges是组成面的逆时针最后一条边
    Edge* nEdges[3];
    Edge* cEdges[3];
    Edge* oEdges[3];

    for (int i = 0; i < 3; i++) {
        nEdges[i] = newEdge(v, fEdge->from);
        cEdges[i] = newEdge(fEdge->to, v);
        oEdges[i] = fEdge;
        fEdge = fEdge->next;
    }

    for (int i = 0; i < 3; i++) {
        constructFace(nFaces[i], nEdges[i], oEdges[i], cEdges[i]);
        nEdges[i]->twin = cEdges[(i + 2) % 3];
        cEdges[(i + 2) % 3]->twin = nEdges[i];
    }

    std::vector<Vertex*> children(nFaces[0]->owned.begin(), nFaces[0]->owned.end());
    for (int i = 0; i < 3; i++) {
        nFaces[i]->owned.clear();
    }

    for (auto vs : children) {
        if (vs == v) continue;
        for (int i = 0; i < 3; i++) {
            if (vs->testInTriangle(nFaces[i]))break;
        }
    }

    int sum = 0;
    for (int i = 0; i < 3; i++) {
        sum += nFaces[i]->owned.size();
    }
    assert(sum <= children.size());

    std::queue<Edge*> Q;
    for (int i = 0; i < 3; i++) {
        Q.push(oEdges[i]);
    }

    while (!Q.empty()) {
        auto curEdge = Q.front();
        Q.pop();

        auto twin = curEdge->twin;
        if (!twin) continue;

        auto targetV = twin->next->to;
        //assert(targetV != v);
        if (targetV == v) continue;

        if (inCircumcircle(curEdge->from, curEdge->to, v, targetV)) {
            auto nxt1 = twin->next;
            // Add suspectiable edge
            Q.push(nxt1);
            Q.push(nxt1->next);

            auto selfFace = curEdge->face;
            auto otherFace = twin->face;

            assert(selfFace != otherFace);
            children.clear();
            for (auto vs : selfFace->owned) {
                children.push_back(vs);
            }
            for (auto vs : otherFace->owned) {
                children.push_back(vs);
            }
            selfFace->owned.clear();
            otherFace->owned.clear();



            auto lastNxt = curEdge->next;
            auto lastPrev = curEdge->prev;

            // Do edge flip
            curEdge->from = v;
            curEdge->to = targetV;

            twin->from = targetV;
            twin->to = v;

            constructFace(selfFace, curEdge, nxt1->next, lastNxt);
            constructFace(otherFace, twin, lastPrev, nxt1);

            for (auto vs : children) {
                if (vs == v) continue;
                if (vs->testInTriangle(selfFace)) continue;
                vs->testInTriangle(otherFace);
            }

        }
    }
}
