#include <corgi/test/test.h>

using namespace corgi::test;

class TestEntity : public Test
{
public:

	void set_up() override
	{
		
	}

	void tear_down() override
	{
		
	}

private:

};


TEST_F(TestEntity, Stuff)
{
	assert_that(true, equals(true))
}