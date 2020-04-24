Naming:

	Classes/Structs/Enums: proper case

	Private Class Members: snake case with m_XX
	Public Class Members: proper case

	Private Static Class Members: snake case with s_XX
	Public Static Class Members: proper case


	Public Class Methods: snake case
	Private Class Methods: snake case

	Public Static Class Methods: proper case
	Private Static Class Methods: snake case?

	Free 'namespace' Functions: snake case?

Classes:

	• Resource classes which encapsulate some sort of handle or computer resource should be 
	  handled with RAII, and should be created only by a public static Create() function
	  which returns a unique pointer to the instance. This way we can allow initialization
	  to fail, and we can guarantee that the class will never be copied which causes issues
	  with RAII.
	




