
#pragma once

namespace draw
{
	namespace components
	{
		/*
		struct Component
		{
			explicit Component(size_t eID):
				m_entityID{eID}
			{
			}

			const size_t entityID() const { return m_entityID; }
			const size_t componentID() const { return m_componentID; }

		protected:
			size_t	m_entityID{ 0 };
			size_t	m_componentID{ m_nextUniqueComponetID++ }; //auto increment

		private:
			//si no le pongo inline, deberia incializarlo afuera
			inline static size_t m_nextUniqueComponetID{ 0 }; 
		};
		*/

		enum TYPE_COMPONENT { TC_BACKBUFFER_DEV, TC_MALLOC_BUFFER, TC_NONE };

		struct Component
		{
			
				explicit Component(int compID) :
					m_componentID{ compID }
				{
				}


				const int componentID() const { return m_componentID; }

			protected:
				int	m_componentID{ TC_BACKBUFFER_DEV };


		};

	}; //component
};//draw