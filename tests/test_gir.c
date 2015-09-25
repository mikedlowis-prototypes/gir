#include "atf.h"
#include "gir.h"
#include "gir_internals.h"

TEST_SUITE(GirTests) {
    /* Verify core object behavior
     *************************************************************************/
    TEST(Constructing a raw object) {
        Obj* obj = obj_new(NULL, 0, 0);
        CHECK(obj != NULL);
        CHECK(obj->hdr.type == 0);
        CHECK(obj->hdr.size == 0);
        CHECK(obj->hdr.parent == NULL);
    }

    TEST(Constructing a raw object with extra space for data) {
        Obj* obj = obj_new(NULL, 1, sizeof(int));
        CHECK(obj != NULL);
        CHECK(obj->hdr.type == 1);
        CHECK(obj->hdr.size == sizeof(int));
        CHECK(obj->hdr.parent == NULL);
    }

    TEST(Adding a slot to an object) {
        Obj* obj = obj_new(NULL, 1, sizeof(int));
        obj_set(obj, 42, obj);
        CHECK(obj != NULL);
        CHECK(obj == obj_get(obj, 42));
    }

    TEST(Set the value of a slot for an object) {
        Obj* obj = obj_new(NULL, 1, sizeof(int));
        obj_set(obj, 42, obj);
        CHECK(obj != NULL);
        CHECK(obj == obj_get(obj, 42));
        obj_set(obj, 42, NULL);
        CHECK(NULL == obj_get(obj, 42));
    }
}
