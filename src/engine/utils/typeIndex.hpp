namespace utils {

	class TypeIndex
	{
		int m_counter = 0;

	public:
		template<typename T>
		int value()
		{
			static int id = m_counter++;
			return id;
		}
	};

}